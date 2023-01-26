#include "patch_utils.h"

// Initialization function called when the DLL is first loaded
void applyPatches()
{
	//patcher::setupPostInitHook();
	//patcher::setupCleanupHook();

}

/* If patcher::setupPostInitHook() is called above, then this function will be called
 * right before entering the main game loop.
 */
void applyPostInitPatches()
{

}

/* If patcher::setupCleanupHook() is called above, then this function will be called
*  upon exiting the game (with Esc+Esc).
*/
void cleanup()
{

}
