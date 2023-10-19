#include "Config.h"
#include <filesystem>
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "doukutsu/window.h"

Config config;

constexpr unsigned short DefaultPort = 5451;
#ifdef NDEBUG
constexpr bool DefaultEnableConsole = false;
constexpr int DefaultLogLevel = 1;
#else
constexpr bool DefaultEnableConsole = true;
constexpr int DefaultLogLevel = 4;
#endif
constexpr bool Default60FPS = false;

Config::Config() : port(DefaultPort), console(DefaultEnableConsole), enable60fps(Default60FPS), maxLogLevel(DefaultLogLevel)
{}

void Config::load(const char* name)
{
	namespace fs = std::filesystem;
	fs::path gamePath{csvanilla::gModulePath};
	fs::path iniPath = gamePath / name;
	if (fs::is_regular_file(iniPath))
	{
		std::string pathString = iniPath.string();
		const char* pathStr = pathString.c_str();
		port = GetPrivateProfileIntA("server", "port", DefaultPort, pathStr);
		console = GetPrivateProfileIntA("debug", "show_console", DefaultEnableConsole, pathStr);
		// Also enable console if "debug" file exists
		console |= fs::is_regular_file(gamePath / "debug");
		enable60fps = GetPrivateProfileIntA("config", "60fps", Default60FPS, pathStr);
		maxLogLevel = GetPrivateProfileIntA("debug", "log_level", DefaultLogLevel, pathStr);
		if (maxLogLevel < 0)
			maxLogLevel = 0;
		else if (maxLogLevel > 4)
			maxLogLevel = 4;
	}
}
