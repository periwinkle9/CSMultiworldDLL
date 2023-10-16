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
