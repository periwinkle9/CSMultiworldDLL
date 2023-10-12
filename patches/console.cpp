#include "console.h"
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <utility>
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "RequestQueue.h"
#include "server/Server.h"

class ConsoleManager
{
	std::thread inputThread;
	std::mutex mutex;
	std::condition_variable cv;
	HHOOK keyboardHook;
	bool shouldExit;
	bool isEnterPressed;
public:
	ConsoleManager();
	~ConsoleManager();

	void handleInputs();
	void tryActivatingInput();
};

ConsoleManager* console = nullptr;

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
		if (command == "kill_server")
		{
			if (tcpServer != nullptr)
			{
				std::cout << "Sending server kill command" << std::endl;
				tcpServer->forceStop();
				std::cout << "Killed the server" << std::endl;
			}
			else
				std::cout << "Server not initialized" << std::endl;
		}
		else if (requestQueue != nullptr)
		{
			RequestQueue::Request request;
			request.type = RequestQueue::Request::RequestType::SCRIPT;
			request.data = std::move(command);
			requestQueue->push(std::move(request));
			std::cout << "Command sent." << std::endl;
		}
		else
			std::cout << "Command receiver not initialized" << std::endl;
	}
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
			std::cerr << "Failed to set keyboard hook! You will be unable to send commands from this console." << std::endl;

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
