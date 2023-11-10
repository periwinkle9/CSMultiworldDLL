#include "patch_utils.h"
#include "patches/game_hooks.h"
#include "patches/Multiworld.h"
#include "doukutsu/misc.h"

// Initialization function called when the DLL is first loaded
void applyPatches()
{
	//patcher::setupPostInitHook();
	patcher::setupCleanupHook();
	applyGameHooks();
	// Increase TSC buffer size (temporary fix until the scripts themselves are revised to fit within the vanilla limit)
	const patcher::dword NewTSCBufferSize = 0xA000; // Double the vanilla limit should be enough
	patcher::patchBytes(0x421545, reinterpret_cast<const patcher::byte*>(&NewTSCBufferSize), sizeof NewTSCBufferSize);
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
	csmulti::Multiworld::end();
}
