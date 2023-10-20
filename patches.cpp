#include "patch_utils.h"
#include "patches/console.h"
#include "patches/Config.h"
#include "patches/Logger.h"
#include "patches/tsc/TSCExecutor.h"
#include "patches/server/Server.h"
#include "patches/RequestQueue.h"
#include "patches/game_hooks.h"
#include "patches/uuid.h"
#include "doukutsu/misc.h"

// Initialization function called when the DLL is first loaded
void applyPatches()
{
	patcher::setupPostInitHook();
	patcher::setupCleanupHook();
	applyGameHooks();
}

/* If patcher::setupPostInitHook() is called above, then this function will be called
 * right before entering the main game loop.
 */
void applyPostInitPatches()
{
	using namespace csmulti;
	config.load("settings.ini");
	if (config.use60fps())
		patch60fps();
	logger.setLogLevel(static_cast<Logger::LogLevel>(config.logLevel()));
	loadUUID();
	initRequestQueue();
	initTSC2();
	if (config.enableConsole())
		initConsole();
	initServer();
}

/* If patcher::setupCleanupHook() is called above, then this function will be called
*  upon exiting the game (with Esc+Esc).
*/
void cleanup()
{
	endServer();
	exitConsole();
	endTSC2();
	endRequestQueue();
}
