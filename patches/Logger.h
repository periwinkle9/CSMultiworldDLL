#pragma once

#include <string>
#include <utility>
#include <atomic>

class Logger
{
public:
	enum class LogLevel { None, Error, Warning, Info, Debug };
private:
	std::atomic<LogLevel> logLevel;
	std::atomic_bool isUsingStdout;
	std::atomic_bool isShowingTimestamps;

	void log(LogLevel level, std::string message);
public:
	Logger(LogLevel level = LogLevel::None);
	bool useStdout(bool use) { bool old = isUsingStdout.exchange(use); return old; }
	bool showTimestamps(bool show) { bool old = isShowingTimestamps.exchange(show); return old; }
	void logError(std::string message) { log(LogLevel::Error, std::move(message)); }
	void logWarning(std::string message) { log(LogLevel::Warning, std::move(message)); }
	void logInfo(std::string message) { log(LogLevel::Info, std::move(message)); }
	void logDebug(std::string message) { log(LogLevel::Debug, std::move(message)); }
	void setLogLevel(LogLevel level) { logLevel = level; }
};

extern Logger logger;