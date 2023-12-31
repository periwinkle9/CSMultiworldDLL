#include "console.h"
#include <iostream>
#include <sstream>
#include <string>
#include <format>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <utility>
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Multiworld.h"
#include "server/Server.h"
#include "doukutsu/flags.h"

using csmulti::logger;
using csmulti::eventQueue;
using csmulti::tcpServer;

class ConsoleManager
{
	std::thread inputThread;
	std::mutex mutex;
	std::condition_variable cv;
	HHOOK keyboardHook;
	bool shouldExit;
	bool isEnterPressed;
	static void handleCommand(std::string command);
public:
	ConsoleManager();
	~ConsoleManager();

	void handleInputs();
	void tryActivatingInput();
};

ConsoleManager* console = nullptr;

// Delay initialization of console to avoid calling the constructor from within DllMain()
void initConsole()
{
	console = new ConsoleManager;
}

void exitConsole()
{
	delete console;
	console = nullptr;
}

void ConsoleManager::handleInputs()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(mutex);
		cv.wait(lock, [this]() { return isEnterPressed || shouldExit; });
		if (shouldExit)
			break;

		// Handle input
		isEnterPressed = false;
		bool prevLoggerStdout = logger().useStdout(false); // Pause console logging while a command is being entered

		std::cout << "Enter command: ";
		std::string command;
		while (true)
		{
			std::string line;
			std::getline(std::cin >> std::ws, line);
			command += line;
			if (!line.empty() && line.back() == '\\')
			{
				command.pop_back();
				command += "\r\n";
				std::cout << "> ";
			}
			else
				break;
		}
		if (!command.empty())
		{
			if (command[0] == '/')
				handleCommand(std::move(command));
			else
			{
				eventQueue().push(std::move(command));
				std::cout << "Command sent." << std::endl;
			}
		}

		logger().useStdout(prevLoggerStdout);
	}
}

void ConsoleManager::handleCommand(std::string command)
{
	std::istringstream iss(command);
	std::string cmd;
	iss >> cmd;
	if (cmd == "/kill_server")
	{
		if (tcpServer() != nullptr)
		{
			std::cout << "Sending server kill command" << std::endl;
			tcpServer()->forceStop();
			std::cout << "Killed the server" << std::endl;
		}
		else
			std::cout << "Server not initialized" << std::endl;
	}
	else if (cmd == "/get_flag")
	{
		int flagNum = 0;
		iss >> flagNum;
		std::cout << std::format("Flag {}: {}", flagNum, csvanilla::GetNPCFlag(flagNum)) << std::endl;
	}
	else if (cmd == "/log_level")
	{
		int newLogLevel = 0;
		if (iss >> newLogLevel)
		{
			if (newLogLevel < 0)
				newLogLevel = 0;
			else if (newLogLevel >= static_cast<int>(csmulti::Logger::LogLevel::Max))
				newLogLevel = static_cast<int>(csmulti::Logger::LogLevel::Max) - 1;
			logger().setLogLevel(static_cast<csmulti::Logger::LogLevel>(newLogLevel));
			std::cout << "Set log level to " << newLogLevel << std::endl;
		}
		else
			std::cout << "Current log level is " << static_cast<int>(logger().getLogLevel()) << std::endl;
	}
	else if (cmd == "/log_file")
	{
		bool currentSetting = logger().isLoggingToFile();
		std::string newValueStr;
		if (iss >> newValueStr)
		{
			bool newValue = newValueStr == "1" || newValueStr == "true";
			if (newValue)
			{
				if (currentSetting)
					std::cout << "Logging to file is already enabled" << std::endl;
				else
				{
					logger().logToFile(true);
					std::cout << "Logging to file enabled" << std::endl;
				}
			}
			else
			{
				if (currentSetting)
				{
					logger().logToFile(false);
					std::cout << "Logging to file disabled" << std::endl;
				}
				else
					std::cout << "Logging to file is already disabled" << std::endl;
			}
		}
		else
			std::cout << "Logging to file is currently " << (currentSetting ? "enabled" : "disabled") << std::endl;
	}
	else if (cmd == "/log_timestamps")
	{
		std::string newValueStr;
		bool newValue;
		newValue = !(iss >> newValueStr) || newValueStr == "1" || newValueStr == "true";
		logger().showTimestamps(newValue);
		if (newValue)
			std::cout << "Enabled log timestamps" << std::endl;
		else
			std::cout << "Disabled log timestamps" << std::endl;
	}
	else
		std::cout << "Unrecognized command" << std::endl;
}

LRESULT CALLBACK keyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0 && wParam == VK_RETURN && (HIWORD(lParam) & KF_ALTDOWN) != 0)
		console->tryActivatingInput();
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

ConsoleManager::ConsoleManager() : inputThread{}, mutex{}, cv{}, keyboardHook{nullptr}, shouldExit{false}, isEnterPressed{false}
{
	if (AllocConsole())
	{
		// Reopen standard I/O streams to work with this console
		FILE* dummy; // Shuts up compiler warnings
		freopen_s(&dummy, "CONOUT$", "w", stdout);
		freopen_s(&dummy, "CONOUT$", "w", stderr);
		freopen_s(&dummy, "CONIN$", "r", stdin);

		// Install keyboard hook
		keyboardHook = SetWindowsHookExA(WH_KEYBOARD, keyboardHookProc, nullptr, GetCurrentThreadId());
		if (keyboardHook == nullptr)
		{
			std::cerr << "Failed to set keyboard hook! You will be unable to send commands from this console." << std::endl;
		}

		inputThread = std::thread(std::bind(&ConsoleManager::handleInputs, this));
	}
}

ConsoleManager::~ConsoleManager()
{
	if (keyboardHook != nullptr)
		UnhookWindowsHookEx(keyboardHook);
	{
		std::unique_lock<std::mutex> lock(mutex);
		shouldExit = true;
	}
	cv.notify_one();
	inputThread.join();
}

void ConsoleManager::tryActivatingInput()
{
	{
		std::unique_lock<std::mutex> lock(mutex, std::try_to_lock);
		if (lock)
			isEnterPressed = true;
	}
	cv.notify_one();
}
