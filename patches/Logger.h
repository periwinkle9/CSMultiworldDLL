#pragma once

#include <string>
#include <utility>
#include <atomic>

namespace csmulti
{
class Logger
{
public:
	enum class LogLevel { None, Error, Warning, Info, Debug };
private:
	std::atomic<LogLevel> logLevel;
	std::atomic_bool isUsingStdout;
	std::atomic_bool isShowingTimestamps;

public:
	Logger(LogLevel level = LogLevel::None);

	void log(LogLevel level, std::string message);
	void logError(std::string message) { log(LogLevel::Error, std::move(message)); }
	void logWarning(std::string message) { log(LogLevel::Warning, std::move(message)); }
	void logInfo(std::string message) { log(LogLevel::Info, std::move(message)); }
	void logDebug(std::string message) { log(LogLevel::Debug, std::move(message)); }

	bool useStdout(bool use) { bool old = isUsingStdout.exchange(use); return old; }
	bool showTimestamps(bool show) { bool old = isShowingTimestamps.exchange(show); return old; }
	void setLogLevel(LogLevel level) { logLevel = level; }
	LogLevel getLogLevel() const { return logLevel.load(); }
};
} // end namespace csmulti
