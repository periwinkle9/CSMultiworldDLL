#include "Multiworld.h"
#include "request/RequestQueue.h"
#include "server/Server.h"
#include "tsc/TSCExecutor.h"
#include "console.h"
#include "game_hooks.h"

namespace csmulti
{
void Multiworld::init()
{
	config_.load("settings.ini");
	logger_.setLogLevel(static_cast<Logger::LogLevel>(config_.logLevel()));
	uuid_.load();
	if (config_.enableConsole())
	{
		initConsole();
		logger_.useStdout(true);
	}
	if (config_.use60fps())
		patch60fps();
	server_ = new Server{config_.serverPort()};
}
void Multiworld::deinit()
{
	if (server_ != nullptr)
	{
		server_->stop();
		delete server_;
		server_ = nullptr;
	}
	exitConsole();
}
} // end namespace csmulti
