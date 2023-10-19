#include "game_hooks.h"
#include "patch_utils.h"
#include "tsc/TSCExecutor.h"
#include "RequestQueue.h"
#include "uuid.h"
#include "doukutsu/inventory.h"
#include "doukutsu/map.h"
#include "doukutsu/misc.h"
#include "doukutsu/profile.h"
#include "doukutsu/teleporter.h"
#include "doukutsu/tsc.h"
#include "doukutsu/window.h"

namespace
{

using Request = RequestQueue::Request;

void handleRequest(const Request& request)
{
	switch (request.type)
	{
	case Request::RequestType::SCRIPT:
		secondaryTSCParser->runScript(std::any_cast<std::string>(request.data));
		break;
	case Request::RequestType::EVENTNUM:
		secondaryTSCParser->runEvent(std::any_cast<int>(request.data));
		break;
	}
}

// Replaces the call to TextScriptProc() in ModeAction()
int TextScriptProcWrapper()
{
	if (secondaryTSCParser != nullptr)
	{
		if (!secondaryTSCParser->isRunning())
		{
			Request request;
			if (requestQueue != nullptr && requestQueue->tryPopTSC(request))
				handleRequest(request);
		}
		secondaryTSCParser->tick();
	}
	// Run vanilla TSC
	return csvanilla::TextScriptProc();
}

// Replaces the call to PutTextScript() in ModeAction()
void PutTextScriptWrapper()
{
	csvanilla::PutTextScript();
	if (secondaryTSCParser != nullptr)
		secondaryTSCParser->draw();
}

// Replaces the call to SystemTask()
int SystemTaskWrapper()
{
	if (requestQueue != nullptr)
		requestQueue->fulfillAll();
	return csvanilla::SystemTask();
}

void* ProfileUUID = &csvanilla::gMapping[sizeof csvanilla::gMapping - sizeof uuid]; // Use the last 16 bytes of map flag space

// Replacement for vanilla StartMapping()
void StartMapping()
{
	using csvanilla::gMapping;
	std::memset(gMapping, 0, sizeof gMapping);
	if (uuidInitialized)
		std::memcpy(ProfileUUID, &uuid, sizeof uuid);
}

BOOL LoadProfile(const char* name)
{
	if (!csvanilla::LoadProfile(name))
		return FALSE;
	if (uuidInitialized && std::memcmp(ProfileUUID, &uuid, sizeof uuid) != 0)
	{
		MessageBoxA(csvanilla::ghWnd, "The save file you are loading appears to have been created from a different world.\n"
			"A new game will be started instead.", "UUID Mismatch", MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	}
	// Clear TSC script queue and stop running events on game reset
	if (requestQueue != nullptr)
		requestQueue->clearTSCQueue();
	if (secondaryTSCParser != nullptr)
		secondaryTSCParser->endEvent();
	return TRUE;
}

BOOL InitializeGame(void* hWnd)
{
	// Clear TSC script queue and stop running events  on new game
	if (requestQueue != nullptr)
		requestQueue->clearTSCQueue();
	if (secondaryTSCParser != nullptr)
		secondaryTSCParser->endEvent();
	return csvanilla::InitializeGame(hWnd);
}

} // end anonymous namespace

void patchUUIDChecks();

void applyGameHooks()
{
	applyTSCHooks();
	patchUUIDChecks();
	hookGameLoops();
	patcher::writeCall(0x40B34C, SystemTaskWrapper);
	const patcher::dword InitalizeGameCalls[] = {0x410497, 0x4104B0, 0x424D52, 0x424E1B};
	for (auto addr : InitalizeGameCalls)
		patcher::writeCall(addr, InitializeGame);
}

void applyTSCHooks()
{
	// Hook into TextScriptProc() and PutTextScript() in ModeAction()
	// (We don't run the secondary TSC parser anywhere else because it's not an appropriate time to receive items)
	patcher::writeCall(0x4107EE, TextScriptProcWrapper);
	patcher::writeCall(0x41086F, PutTextScriptWrapper);
}

void patchUUIDChecks()
{
	patcher::replaceFunction(csvanilla::StartMapping, StartMapping);
	const patcher::dword LoadProfileCalls[] = {0x410487, 0x412C7A, 0x424E08};
	for (auto addr : LoadProfileCalls)
		patcher::writeCall(addr, LoadProfile);
}

void patch60fps()
{
	// This is just the 60fps.xml hack
	const patcher::byte patch60fps[] = {
		0x55, 0x89, 0xe5, 0x53, 0x31, 0xdb, 0xb9, 0x3c, 0xd4, 0x49, 0x00, 0x8b, 0x01, 0xff, 0x01, 0x31,
		0xd2, 0x6a, 0x03, 0x59, 0xf7, 0xf1, 0x85, 0xd2, 0x0f, 0x95, 0xc3, 0x83, 0xc3, 0x10, 0xeb, 0x1c,
		0xff, 0x15, 0x58, 0xc2, 0x48, 0x00, 0x89, 0xc2, 0xa1, 0x38, 0xd4, 0x49, 0x00, 0x8d, 0x0c, 0x18,
		0x39, 0xca, 0x73, 0x19, 0x6a, 0x01, 0xff, 0x15, 0x20, 0xc1, 0x48, 0x00, 0xe8, 0xef, 0x81, 0x00,
		0x00, 0x85, 0xc0, 0x75, 0xdb, 0x31, 0xc0, 0x5b, 0xe9, 0xbd, 0x00, 0x00, 0x00, 0x83, 0xc0, 0x64,
		0x39, 0xc2, 0x0f, 0x43, 0xca, 0x89, 0x0d, 0x38, 0xd4, 0x49, 0x00, 0x5b, 0x90, 0x90, 0x90
	};
	constexpr patcher::dword Flip_SystemTask = 0x40B340; // Too lazy to #include "doukutsu/draw.h" just for this
	patcher::patchBytes(Flip_SystemTask, patch60fps, sizeof patch60fps);
	// Redirect the SystemTask() call to use our wrapper
	patcher::writeCall(0x40B37C, SystemTaskWrapper);

	// Let's patch the Nikumaru Counter too, while we're at it
	const patcher::byte timerPatch[] = {0x10, 0x0E, 0x3C, 0x06};
	patcher::patchBytes(0x41A530, timerPatch, 2);
	patcher::patchBytes(0x41A550, timerPatch + 2, 1);
	patcher::patchBytes(0x41A57B, timerPatch + 3, 1);
}

// I hope this is safe to initialize from within DllMain()...
std::atomic<GameMode> currentGameMode = GameMode::INIT;
#define MAKE_FUNC(name, args, mode, ...) int name args \
{ \
	GameMode prevGameMode = currentGameMode.exchange(mode); \
	int ret = csvanilla:: name (__VA_ARGS__); \
	currentGameMode = prevGameMode; \
	return ret; \
}

// Workaround for wrapping ModeTitle() and ModeAction() which contain ASM hacks that clobber ESI,
// which also causes issues with using the approach in MAKE_FUNC above
#define MAKE_GAME_FUNC(name, mode) int name(void* hWnd) \
{ \
	currentGameMode = mode; \
	return csvanilla::name(hWnd); \
}

namespace
{
MAKE_FUNC(ModeOpening, (void* hWnd), GameMode::OPENING, hWnd)

// MSVC's runtime stack frame checks work by saving ESP to ESI before a function call and then checking that ESP was restored correctly after the call.
// These functions contain hacks that use ESI without preserving its value. This will spuriously trigger the runtime stack frame check since ESI will
// no longer be equal to ESP, but in reality there is no stack frame error so this check can safely be disabled for these functions.
#pragma runtime_checks("s", off)
MAKE_GAME_FUNC(ModeTitle, GameMode::TITLE)
MAKE_GAME_FUNC(ModeAction, GameMode::ACTION)
#pragma runtime_checks("s", restore)

// Replaces the EndMapData() call when exiting (sets currentGameMode correctly, since the MAKE_GAME_FUNC workaround doesn't do this)
void DeinitHook()
{
	currentGameMode = GameMode::INIT;
	return csvanilla::EndMapData();
}

MAKE_FUNC(CampLoop, (), GameMode::INVENTORY)
MAKE_FUNC(StageSelectLoop, (int* event), GameMode::TELEPORTER, event)
MAKE_FUNC(MiniMapLoop, (), GameMode::MINIMAP)
MAKE_FUNC(Scene_DownIsland, (void* hWnd, int mode), GameMode::ISLAND_FALLING, hWnd, mode)
MAKE_FUNC(Call_Escape, (void* hWnd), GameMode::ESCAPE, hWnd)
}

void hookGameLoops()
{
	using patcher::writeCall;
	writeCall(0x40F6A7, ModeOpening);
	writeCall(0x40F6BC, ModeTitle);
	writeCall(0x40F6D1, ModeAction);
	writeCall(0x40F6EA, DeinitHook);
	writeCall(0x410725, CampLoop);
	writeCall(0x4244CE, StageSelectLoop);
	writeCall(0x410785, MiniMapLoop);
	writeCall(0x42444C, MiniMapLoop);
	writeCall(0x4251B5, Scene_DownIsland);

	const patcher::dword EscapeAddrs[] = {0x401DF2, 0x40DC2A, 0x40F7CD, 0x40FF6A, 0x4104E8, 0x4146D8, 0x41488B, 0x4149F0, 0x41DAA0};
	for (patcher::dword addr : EscapeAddrs)
		writeCall(addr, Call_Escape);
}
