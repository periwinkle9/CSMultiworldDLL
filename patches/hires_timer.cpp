// Timer implementation largely based off of SDL2's SDL_GetTicks() implementation
// Any system running Windows XP or later should in theory be able to use
// QueryPerformanceCounter(), but just for the heck of it let's keep a fallback
// in case somebody wants to try running this on an ancient PC. :P

// No WIN32_LEAN_AND_MEAN here, that disables the declaration of timeGetTime()
#define ALL_WIN_H
#include "windows_h_wrapper.h"
#include "patch_utils.h"

namespace hires_timer
{

bool inited = false;
bool hasHiResTimer;
LARGE_INTEGER startTicks;
LARGE_INTEGER tickFreq;

void TimerInit()
{
	if (QueryPerformanceFrequency(&tickFreq))
	{
		hasHiResTimer = true;
		QueryPerformanceCounter(&startTicks);
	}
	else
		hasHiResTimer = false;
	inited = true;
}

DWORD GetTicks()
{
	if (!inited)
		TimerInit();
	if (hasHiResTimer)
	{
		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);

		time.QuadPart -= startTicks.QuadPart;
		time.QuadPart = (time.QuadPart * 1000) / tickFreq.QuadPart;

		return static_cast<DWORD>(time.QuadPart);
	}
	else
		return timeGetTime();
}

void applyPatch()
{
	// Replace the call to GetTickCount() with our GetTicks() function
	patcher::writeCall(0x40B35C, hires_timer::GetTicks);
	patcher::writeNOPs(0x40B361, 1); // NOP the 1 extra byte left over
}

}