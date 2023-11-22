#include "Multiworld.h"
#include "server/Server.h"
#include "console.h"

static bool isSoloModeUUID(const IID& uuid)
{
	// UUID is always {00000000-0000-1111-0000-000000000000} for solo mode seeds
	if (uuid.Data1 != 0 || uuid.Data2 != 0 || uuid.Data3 != 0x1111)
		return false;
	for (int i = 0; i < 8; ++i)
		if (uuid.Data4[i] != 0)
			return false;
	return true;
}

namespace csmulti
{
Multiworld* Multiworld::instance = nullptr;
void Multiworld::init()
{
	config_.load("settings.ini");
	logger_.setLogLevel(static_cast<Logger::LogLevel>(config_.logLevel()));
	logger_.logToFile(config_.logToFile());
	uuid_.load();
	if (config_.enableConsole())
	{
		initConsole();
		logger_.useStdout(true);
	}

	if (config_.enableServer())
	{
		if (!(isSoloModeUUID(uuid_.get()) && !config_.enableServerInSolo()))
			server_ = new Server{config_.serverPort()};
		else
			logger_.logInfo("Server disabled due to solo mode seed and disable_if_solo_seed=1");
	}
	else
		logger_.logInfo("Server disabled due to start_server=0");
}
void Multiworld::deinit()
{
	requestQueue_.cancelAll();
	if (server_ != nullptr)
	{
		server_->stop();
		delete server_;
		server_ = nullptr;
	}
	exitConsole();
}

void Multiworld::clearRequestsAndTSC()
{
	requestQueue_.cancelAll();
	tscQueue_.clear();
	tscParser_.endEvent();
}
} // end namespace csmulti
