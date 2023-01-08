#include "patch_utils.h"

// Initialization function called when the DLL is first loaded
void applyPatches()
{
	//patcher::setupPostInitHook();


}

/* If patcher::setupPostInitHook() is called above, then this function will be called
 * right before entering the main game loop.
 */
void applyPostInitPatches()
{

}
