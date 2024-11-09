#include "game_hooks.h"
#include <cstring>
#include <string>
#include <stdexcept>
#include "patch_utils.h"
#include "Multiworld.h"
#include "doukutsu/map.h"
#include "doukutsu/misc.h"
#include "doukutsu/profile.h"
#include "doukutsu/tsc.h"
#include "doukutsu/window.h"

using namespace csmulti;

namespace
{

using Event = TSCQueue::Event;

void runEvent(const Event& request)
{
	switch (request.index())
	{
	case 0: // std::string
		secondaryTSCParser().runScript(std::get<0>(request));
		break;
	case 1: // int
		secondaryTSCParser().runEvent(std::get<1>(request));
		break;
	}
}

// Replaces the call to TextScriptProc() in ModeAction()
int TextScriptProcWrapper()
{
	if (!secondaryTSCParser().isRunning())
	{
		Event event;
		if (eventQueue().tryPop(event))
			runEvent(event);
	}
	secondaryTSCParser().tick();
	// Run vanilla TSC
	return csvanilla::TextScriptProc();
}

// Replaces the call to PutTextScript() in ModeAction()
void PutTextScriptWrapper()
{
	csvanilla::PutTextScript();
	secondaryTSCParser().draw();
}

// Replaces the call to SystemTask()
int SystemTaskWrapper()
{
	try
	{
		requestQueue().fulfillAll();
	}
	catch (const std::exception& e)
	{
		using namespace std::string_literals;
		logger().logError("Request handler threw exception: "s + e.what());
		return 0;
	}
	return csvanilla::SystemTask();
}

void* ProfileUUID = &csvanilla::gMapping[sizeof csvanilla::gMapping - sizeof(IID)]; // Use the last 16 bytes of map flag space

// Replacement for vanilla StartMapping()
void StartMapping()
{
	using csvanilla::gMapping;
	std::memset(gMapping, 0, sizeof gMapping);
	if (uuid().isInitialized())
		std::memcpy(ProfileUUID, &uuid().get(), sizeof(IID));
}

BOOL LoadProfile(const char* name)
{
	if (!csvanilla::LoadProfile(name))
		return FALSE;
	if (uuid().isInitialized() && std::memcmp(ProfileUUID, &uuid().get(), sizeof(IID)) != 0 && !config().ignoreUUIDMismatch())
	{
		MessageBoxA(csvanilla::ghWnd, "The save file you are loading appears to have been created from a different world.\n"
			"A new game will be started instead.", "UUID Mismatch", MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	}
	// Clear request queues and stop running events on game reset
	Multiworld::getInstance().clearRequestsAndTSC();
	return TRUE;
}

BOOL InitializeGame(void* hWnd)
{
	// Clear request queues and stop running events on new game
	Multiworld::getInstance().clearRequestsAndTSC();
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

namespace
{
int ModeOpening(void* hWnd)
{
	Multiworld::getInstance().setGameMode(GameMode::NONE);
	return csvanilla::ModeOpening(hWnd);
}

// MSVC's runtime stack frame checks work by saving ESP to ESI before a function call and then checking that ESP was restored correctly after the call.
// ModeTitle() and ModeAction() call functions that contain hacks that use ESI without preserving its value.
// This will spuriously trigger the runtime stack frame check since ESI will no longer be equal to ESP,
// but in reality there is no stack frame error so this check can safely be disabled for these functions.
#pragma runtime_checks("s", off)
int ModeAction(void* hWnd)
{
	Multiworld::getInstance().setGameMode(GameMode::IN_GAME);
	return csvanilla::ModeAction(hWnd);
}
#pragma runtime_checks("s", restore)

// No need to hook ModeTitle() since the game never enters ModeTitle() directly after ModeAction()
}

void hookGameLoops()
{
	using patcher::writeCall;
	writeCall(0x40F6A7, ModeOpening);
	writeCall(0x40F6D1, ModeAction);
}
