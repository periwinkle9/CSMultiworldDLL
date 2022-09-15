// An example of modding via completely replacing vanilla functions with custom code
#include "patch_60fps.h"

// For patching functions
#include "patch_utils.h"

// For rendering code declarations
#include "doukutsu/draw.h"
// For the SystemTask() declaration
#include "doukutsu/window.h"
// For the Restore[X] function declarations
#include "doukutsu/credits.h"
#include "doukutsu/map.h"
#include "doukutsu/tsc.h"
// ...or you can just do this instead of #include-ing all of the above
//#include "doukutsu/all.h"

#include "../hires_timer.h"

// Wrapping your code inside a namespace helps with avoiding name collisions with other patches
namespace cs_60fps_patch
{

BOOL Flip_SystemTask(HWND hWnd)
{
	// Put this at the top of the function and now you can call vanilla functions
	// without having to prefix them with csvanilla::
	using namespace csvanilla;

	static DWORD timePrev = 0;
	static DWORD timeNow;
	static const int frameDelays[] = {17, 16, 17};
	static int idx = 0;

	idx = (idx + 1) % 3;

	while (TRUE)
	{
		if (!SystemTask())
			return FALSE;

		// Framerate limiter
		timeNow = hires_timer::GetTicks(); // Use our high-resolution timer implementation

		if (timeNow >= timePrev + frameDelays[idx])
			break;

		Sleep(1);
	}

	if (timeNow >= timePrev + 100)
		timePrev = timeNow;
	else
		timePrev += frameDelays[idx];

	static ::RECT dst_rect; // TODO: figure out a good resolution to RECT being ambiguous here
	GetWindowRect(hWnd, &dst_rect);
	dst_rect.left += client_x;
	dst_rect.top += client_y;
	dst_rect.right = dst_rect.left + scaled_window_width;
	dst_rect.bottom = dst_rect.top + scaled_window_height;

	frontbuffer->Blt(&dst_rect, backbuffer, &backbuffer_rect, DDBLT_WAIT, NULL);

	if (RestoreSurfaces())
	{
		RestoreStripper();
		RestoreMapName();
		RestoreTextScript();
	}

	return TRUE;
}

void patchTimer()
{
	using namespace patcher;
	// Patch time computation
	dword value = 3600;
	const byte* ptr = reinterpret_cast<byte*>(&value);
	patchBytes(0x41A530, ptr, 4);
	value = 60;
	patchBytes(0x41A550, ptr, 4);
	value = 6;
	patchBytes(0x41A57B, ptr, 4);

	// Patch title screen character thresholds
	value = 6 * 60 * 60; // 6 minutes in frames
	patchBytes(0x40FE09, ptr, 4);
	value = 5 * 60 * 60; // 5 minutes
	patchBytes(0x40FE28, ptr, 4);
	value = 4 * 60 * 60; // 4 minutes
	patchBytes(0x40FE47, ptr, 4);
	value = 3 * 60 * 60; // 3 minutes
	patchBytes(0x40FE66, ptr, 4);
}

void applyPatch()
{
	// Replace OG Flip_SystemTask with our version
	patcher::replaceFunction(csvanilla::Flip_SystemTask, Flip_SystemTask);
	patchTimer();
}

}