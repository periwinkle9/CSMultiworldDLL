#include "console.h"
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <stdio.h>
#include "../windows_h_wrapper.h"

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
		std::string line;
		std::getline(std::cin >> std::ws, line);
		std::cout << "You entered: " << line << std::endl;
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
