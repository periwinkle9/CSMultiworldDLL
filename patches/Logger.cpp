#include "Logger.h"
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifdef NDEBUG
constexpr bool DefaultStdout = false;
constexpr Logger::LogLevel DefaultLogLevel = Logger::LogLevel::Error;
#else
constexpr bool DefaultStdout = true;
constexpr Logger::LogLevel DefaultLogLevel = Logger::LogLevel::Debug;
#endif

Logger logger{DefaultLogLevel};

Logger::Logger(LogLevel level) : isUsingStdout{DefaultStdout}, logLevel{level}
{}

void Logger::log(LogLevel level, std::string message)
{
	if (static_cast<int>(logLevel.load()) < static_cast<int>(level))
		return;

	switch (level)
	{
	case LogLevel::Error:
		message = "[ERROR] " + message;
		break;
	case LogLevel::Warning:
		message = "[WARNING] " + message;
		break;
	case LogLevel::Info:
		message = "[INFO] " + message;
		break;
	case LogLevel::Debug:
		message = "[DEBUG] " + message;
		break;
	}

	if (isUsingStdout)
		std::cout << message << std::endl;
	OutputDebugStringA(message.c_str());
}
