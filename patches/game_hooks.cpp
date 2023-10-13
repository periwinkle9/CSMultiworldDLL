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

// Replaces every call to TextScriptProc()
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

// Replaces every call to PutTextScript()
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
	// Clear TSC script queue on game reset
	if (requestQueue != nullptr)
		requestQueue->clearTSCQueue();
	return TRUE;
}

BOOL InitializeGame(void* hWnd)
{
	// Clear TSC script queue on new game
	if (requestQueue != nullptr)
		requestQueue->clearTSCQueue();
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
	const patcher::dword TextScriptProcCalls[] = {0x4107EE}; /* {0x401E34, 0x40F8EE, 0x4107EE, 0x41DAD8} */;
	for (patcher::dword address : TextScriptProcCalls)
		patcher::writeCall(address, TextScriptProcWrapper);
	const patcher::dword PutTextScriptCalls[] = {0x41086F}; /* {0x401E7F, 0x40F91F, 0x41086F, 0x41DB23} */;
	for (patcher::dword address : PutTextScriptCalls)
		patcher::writeCall(address, PutTextScriptWrapper);
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

namespace
{
MAKE_FUNC(ModeOpening, (void* hWnd), GameMode::OPENING, hWnd)

// These functions contain hacks that use ESI without preserving its value
#pragma runtime_checks("s", off)
MAKE_FUNC(ModeTitle, (void* hWnd), GameMode::TITLE, hWnd)
MAKE_FUNC(ModeAction, (void* hWnd), GameMode::ACTION, hWnd)
#pragma runtime_checks("s", restore)

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
	writeCall(0x410725, CampLoop);
	writeCall(0x4244CE, StageSelectLoop);
	writeCall(0x410785, MiniMapLoop);
	writeCall(0x42444C, MiniMapLoop);
	writeCall(0x4251B5, Scene_DownIsland);

	const patcher::dword EscapeAddrs[] = {0x401DF2, 0x40DC2A, 0x40F7CD, 0x40FF6A, 0x4104E8, 0x4146D8, 0x41488B, 0x4149F0, 0x41DAA0};
	for (patcher::dword addr : EscapeAddrs)
		writeCall(addr, Call_Escape);
}
