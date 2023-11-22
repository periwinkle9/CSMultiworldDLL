#pragma once

#include "Config.h"
#include "Logger.h"
#include "uuid.h"
#include "request/RequestQueue.h"
#include "request/TSCQueue.h"
#include "tsc/TSCExecutor.h"

namespace csmulti
{
// Hack to have a scoped enum with implicit conversion
namespace GameMode
{
enum Mode : unsigned char {
	INIT = 0,
	OPENING = 1,
	TITLE = 2,
	ACTION = 4,
	INVENTORY = 8,
	TELEPORTER = 0x10,
	MINIMAP = 0x20,
	ISLAND_FALLING = 0x40,
	ESCAPE = 0x80
};
} // end namespace GameMode
class Multiworld
{
public:
	using GameMode = GameMode::Mode;
private:
	std::atomic_uchar gameMode_;
	Config config_;
	Logger logger_;
	UUID uuid_;
	RequestQueue requestQueue_;
	TSCQueue tscQueue_;
	TSCExecutor tscParser_;
	// Keeping this as a pointer for flexibility (lifetime of server = lifetime of object)
	class Server* server_;

	// Allocating on heap instead of statically to avoid calling the destructor in DllMain()
	static Multiworld* instance;
	void init();
	void deinit();
	Multiworld() : gameMode_{GameMode::INIT}, config_{}, logger_{}, uuid_{}, requestQueue_{}, tscParser_{}, server_{} { init(); }
public:
	Multiworld(const Multiworld&) = delete;
	Multiworld(Multiworld&&) = delete;
	Multiworld& operator=(const Multiworld&) = delete;
	Multiworld& operator=(Multiworld&&) = delete;
	~Multiworld() { deinit(); }
	// Because this is a DLL, initialization and deinitialization are best done outside of DllMain().
	// Hence, these functions are used to manage the lifetime of the Multiworld instance.
	static Multiworld& getInstance() { static Multiworld* runOnce = (instance = new Multiworld); return *instance; }
	static void end() { delete instance; instance = nullptr; }

	void clearRequestsAndTSC();

	GameMode currentGameMode() const { return GameMode{gameMode_.load()}; }
	void enterGameMode(GameMode mode) { gameMode_ |= mode; }
	void exitGameMode(GameMode mode) { gameMode_ &= ~mode; }
	const Config& config() const { return config_; }
	Logger& logger() { return logger_; }
	const UUID& uuid() const { return uuid_; }
	RequestQueue& requestQueue() { return requestQueue_; }
	TSCQueue& eventQueue() { return tscQueue_; }
	TSCExecutor& tscParser() { return tscParser_; }
	Server* tcpServer() { return server_; }
};

// These functions are just here to simplify the syntax
inline const Config& config() { return Multiworld::getInstance().config(); }
inline Logger& logger() { return Multiworld::getInstance().logger(); }
inline const UUID& uuid() { return Multiworld::getInstance().uuid(); }
inline RequestQueue& requestQueue() { return Multiworld::getInstance().requestQueue(); }
inline TSCQueue& eventQueue() { return Multiworld::getInstance().eventQueue(); }
inline TSCExecutor& secondaryTSCParser() { return Multiworld::getInstance().tscParser(); }
inline Server* tcpServer() { return Multiworld::getInstance().tcpServer(); }

} // end namespace csmulti
