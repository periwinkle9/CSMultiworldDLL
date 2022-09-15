#pragma once

// A replacement for the GetTickCount() function that the game calls in
// Flip_SystemTask() to control the framerate.
// The WinAPI GetTickCount() function has a limited resolution (typically
// between 10-16ms, according to Microsoft's documentation), which makes
// the time in between frames to wobble between 15-16 ms and 30-32 ms,
// causing game to feel stuttery.
// This timer implementation should in theory have a much higher resolution,
// which should make the game feel smoother.

namespace hires_timer
{
unsigned long GetTicks();
void applyPatch();
}
