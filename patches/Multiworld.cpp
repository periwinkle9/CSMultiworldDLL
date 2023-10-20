#include "Multiworld.h"
#include "request/RequestQueue.h"
#include "server/Server.h"
#include "tsc/TSCExecutor.h"
#include "console.h"

namespace csmulti
{
void Multiworld::init()
{
	config_.load("settings.ini");
	logger_.setLogLevel(static_cast<Logger::LogLevel>(config_.logLevel()));
	uuid_.load();
	requestQueue_ = new RequestQueue;
	tscParser_ = new TSCExecutor;
	if (config_.enableConsole())
		initConsole();
	server_ = new Server{config_.serverPort()};
}
void Multiworld::deinit()
{
	server_->stop();
	delete server_;
	server_ = nullptr;
	exitConsole();
	delete tscParser_;
	tscParser_ = nullptr;
	delete requestQueue_;
	requestQueue_ = nullptr;
}
} // end namespace csmulti
