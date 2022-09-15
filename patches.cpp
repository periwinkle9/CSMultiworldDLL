// Collect all of your patches in this file to apply them to the game,
// either by writing them in another file and then #include-ing the
// header in this file (recommended),
// or you can just write the patches directly in this file if you're lazy

#include "patch_utils.h"

// Example of importing patches from another file via #include
// (You still have to call the patch functions inside applyPatches() below, of course)
#include "patches/hires_timer.h"
#include "patches/sample_patches/patch_60fps.h"
#include "patches/sample_patches/custom_tsc.h"

// Example of a patch written directly inside this file
void removeCMUstutter()
{
	// Remove the Sleep(100) call that occurs whenever the music is changed
	// (Which is the reason why the game stutters at these moments)
	patcher::writeNOPs(0x41C868, 6);
}

/* Initialization function called when the DLL is first loaded.
 * Add your patches to this function to have them be applied to the game.
 * NOTE: This function is called from DllMain() when the DLL is loaded.
 * As such, there are significant restrictions on what you can safely do within
 * this function. For more information, see:
 * https://docs.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-best-practices
 * If you need to do something more complicated than just editing some
 * bytes with the patcher, or if you're not sure if what you're doing is
 * safe to do within DllMain(), then I would recommend uncommenting the
 * line below and then writing your patches inside applyPostInitPatches()
 * instead of this function.
 */
void applyPatches()
{
	// Uncomment this if you want to do stuff within applyPostInitPatches()
	//patcher::setupPostInitHook();

	// Apply sample patches
	//hires_timer::applyPatch(); // Not needed with the 60 FPS patch
	removeCMUstutter();
	cs_60fps_patch::applyPatch();
	custom_tsc_cmds::applyPatch();
}

/* If patcher::setupPostInitHook() is called above, then this function will be called
 * right before entering the main game loop.
 * Most things will be initialized at this point so do with that whatever you want. :)
 */
void applyPostInitPatches()
{

}
