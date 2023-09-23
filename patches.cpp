#include "patch_utils.h"
#include "patches/console.h"
#include "patches/tsc/TSCExecutor.h"
#include "patches/server/Server.h"
#include "patches/RequestQueue.h"
#include "patches/game_hooks.h"

// Initialization function called when the DLL is first loaded
void applyPatches()
{
	patcher::setupPostInitHook();
	patcher::setupCleanupHook();
	applyTSCHooks();
}

/* If patcher::setupPostInitHook() is called above, then this function will be called
 * right before entering the main game loop.
 */
void applyPostInitPatches()
{
	initConsole();
	initTSC2();
	initRequestQueue();
	tcpServer = new Server(5451);
}

/* If patcher::setupCleanupHook() is called above, then this function will be called
*  upon exiting the game (with Esc+Esc).
*/
void cleanup()
{
	tcpServer->stop();
	delete tcpServer;
	tcpServer = nullptr;
	exitConsole();
	endTSC2();
	endRequestQueue();
}
