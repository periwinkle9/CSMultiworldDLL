#include "game_hooks.h"
#include "patch_utils.h"
#include "tsc/TSCExecutor.h"
#include "RequestQueue.h"
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
