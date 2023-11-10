#include "Logger.h"
#include <iostream>
#include <string_view>
#include <chrono>
#include <format>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Config.h"

namespace csmulti
{
Logger::Logger(LogLevel level) : logFile{}, mutex{}, logLevel{level}, isUsingStdout{false}, isShowingTimestamps{true}
{}

void Logger::log(LogLevel level, std::string message)
{
	if (static_cast<int>(logLevel.load()) < static_cast<int>(level))
		return;

	std::string_view tag{};
	switch (level)
	{
	case LogLevel::Error:
		tag = "[ERROR]";
		break;
	case LogLevel::Warning:
		tag = "[WARNING]";
		break;
	case LogLevel::Info:
		tag = "[INFO]";
		break;
	case LogLevel::Debug:
		tag = "[DEBUG]";
		break;
	}

	std::string formatted;
	if (isShowingTimestamps)
		formatted = std::format("<{:%T}> {} {}", std::chrono::system_clock::now(), tag, message);
	else
		formatted = std::format("{} {}", tag, message);

	{
		std::scoped_lock lock{mutex};
		if (isUsingStdout)
			std::cout << formatted << std::endl;
		if (isUsingLogFile)
			logFile << formatted << std::endl;
		OutputDebugStringA(formatted.c_str());
	}
}

void Logger::logToFile(bool log)
{
	if (log)
	{
		if (logFile.is_open())
			logWarning("Logger::logToFile(true) called but logger is already logging to file");
		else
			logFile.open("log.txt", std::ios::app);
	}
	else if (logFile.is_open())
		logFile.close();
	isUsingLogFile = log;
}
} // end namespace csmulti
