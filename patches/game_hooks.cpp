#include "game_hooks.h"
#include "patch_utils.h"
#include "tsc/TSCExecutor.h"
#include "RequestQueue.h"
#include "doukutsu/inventory.h"
#include "doukutsu/map.h"
#include "doukutsu/misc.h"
#include "doukutsu/teleporter.h"
#include "doukutsu/tsc.h"

namespace
{

using Request = RequestQueue::Request;

void handleRequest(const Request& request)
{
	switch (request.type)
	{
	case Request::RequestType::SCRIPT:
		secondaryTSCParser->runScript(request.script);
		break;
	case Request::RequestType::EVENTNUM:
		secondaryTSCParser->runEvent(request.eventNum);
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
			if (requestQueue != nullptr && requestQueue->tryPop(request))
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

} // end anonymous namespace

void applyTSCHooks()
{
	const patcher::dword TextScriptProcCalls[] = {0x4107EE}; /* {0x401E34, 0x40F8EE, 0x4107EE, 0x41DAD8} */;
	for (patcher::dword address : TextScriptProcCalls)
		patcher::writeCall(address, TextScriptProcWrapper);
	const patcher::dword PutTextScriptCalls[] = {0x41086F}; /* {0x401E7F, 0x40F91F, 0x41086F, 0x41DB23} */;
	for (patcher::dword address : PutTextScriptCalls)
		patcher::writeCall(address, PutTextScriptWrapper);
}


// I hope this is safe to initialize from within DllMain()...
std::atomic<GameMode> currentGameMode = GameMode::INIT;
#define MAKE_FUNC(name, args, mode, ...) int name args \
{ \
	GameMode prevGameMode = currentGameMode; \
	currentGameMode = mode; \
	int ret = csvanilla:: name (__VA_ARGS__); \
	currentGameMode = prevGameMode; \
	return ret; \
}

namespace
{
MAKE_FUNC(ModeOpening, (void* hWnd), GameMode::OPENING, hWnd)
MAKE_FUNC(ModeTitle, (void* hWnd), GameMode::TITLE, hWnd)
MAKE_FUNC(ModeAction, (void* hWnd), GameMode::ACTION, hWnd)
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
	//writeCall(0x40F6BC, ModeTitle); // Not working--ModeTitle() uses ESI without preserving its value
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
