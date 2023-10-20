#pragma once

#include "Config.h"
#include "Logger.h"
#include "uuid.h"

class TSCExecutor;

namespace csmulti
{
class Multiworld
{
public:
	enum class GameMode { INIT = -1, OPENING, TITLE, ACTION, INVENTORY, TELEPORTER, MINIMAP, ISLAND_FALLING, ESCAPE };
private:
	std::atomic<GameMode> gameMode_; // I hope this is safe to initialize from within DllMain()...
	Config config_;
	Logger logger_;
	UUID uuid_;
	// Note: If incorporating this into a non-DLL project, these don't have to be pointers
	// But it does mean I can forward-declare these to reduce dependencies :)
	class RequestQueue* requestQueue_;
	TSCExecutor* tscParser_;
	class Server* server_;

	Multiworld() : gameMode_{GameMode::INIT}, config_ {}, logger_{}, uuid_{}, requestQueue_{}, tscParser_{}, server_{} {}
public:
	Multiworld(const Multiworld&) = delete;
	Multiworld(Multiworld&&) = delete;
	Multiworld& operator=(const Multiworld&) = delete;
	Multiworld& operator=(Multiworld&&) = delete;
	// Because this is a DLL, initialization and deinitialization is best done outside of DllMain(),
	// so I can't use the constructor or destructor here
	~Multiworld() = default;
	void init();
	void deinit();

	static Multiworld& getInstance() { static Multiworld instance; return instance; }

	GameMode currentGameMode() const { return gameMode_.load(); }
	GameMode setGameMode(GameMode newMode) { return gameMode_.exchange(newMode); }
	const Config& config() const { return config_; }
	Logger& logger() { return logger_; }
	const UUID& uuid() const { return uuid_; }
	RequestQueue* requestQueue() { return requestQueue_; }
	TSCExecutor* tscParser() { return tscParser_; }
	Server* tcpServer() { return server_; }
};
} // end namespace csmulti
