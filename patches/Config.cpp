#include "Config.h"
#include <filesystem>
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "doukutsu/window.h"

namespace csmulti
{
constexpr unsigned short DefaultPort = 5451;
constexpr bool DefaultEnableServer = true;
constexpr bool DefaultEnableSoloServer = true;
#ifdef NDEBUG
constexpr bool DefaultEnableConsole = false;
constexpr int DefaultLogLevel = 1;
#else
constexpr bool DefaultEnableConsole = true;
constexpr int DefaultLogLevel = 4;
#endif
constexpr bool Default60FPS = false;
constexpr bool DefaultIgnoreUUID = false;
constexpr Config::TextSettings DefaultTextSettings{84, 33, 82, -18, RGB(0xFF, 0xFF, 0xFE), RGB(0x11, 0x00, 0x22), false};

Config::Config() : port(DefaultPort), server(DefaultEnableServer), enableServerIfSolo(DefaultEnableSoloServer),
	console(DefaultEnableConsole), enable60fps(Default60FPS), ignoreUUID(DefaultIgnoreUUID),
	maxLogLevel(DefaultLogLevel), textConfig{DefaultTextSettings}
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
		server = GetPrivateProfileIntA("server", "start_server", DefaultEnableServer, pathStr);
		enableServerIfSolo = !GetPrivateProfileIntA("server", "disable_if_solo_seed", !DefaultEnableSoloServer, pathStr);

		enable60fps = GetPrivateProfileIntA("config", "60fps", Default60FPS, pathStr);
		textConfig.textX = GetPrivateProfileIntA("config", "text_x", DefaultTextSettings.textX, pathStr);
		textConfig.textY = GetPrivateProfileIntA("config", "text_y", DefaultTextSettings.textY, pathStr);
		textConfig.gitXoff = GetPrivateProfileIntA("config", "git_x_offset", DefaultTextSettings.gitXoff, pathStr);
		textConfig.gitYoff = GetPrivateProfileIntA("config", "git_y_offset", DefaultTextSettings.gitYoff, pathStr);
		unsigned textR = GetPrivateProfileIntA("config", "text_color_r", GetRValue(DefaultTextSettings.textColor), pathStr) & 0xFF;
		unsigned textG = GetPrivateProfileIntA("config", "text_color_g", GetGValue(DefaultTextSettings.textColor), pathStr) & 0xFF;
		unsigned textB = GetPrivateProfileIntA("config", "text_color_b", GetBValue(DefaultTextSettings.textColor), pathStr) & 0xFF;
		textConfig.textColor = RGB(textR, textG, textB);
		unsigned textShadowR = GetPrivateProfileIntA("config", "text_shadow_color_r", GetRValue(DefaultTextSettings.textShadowColor), pathStr) & 0xFF;
		unsigned textShadowG = GetPrivateProfileIntA("config", "text_shadow_color_g", GetGValue(DefaultTextSettings.textShadowColor), pathStr) & 0xFF;
		unsigned textShadowB = GetPrivateProfileIntA("config", "text_shadow_color_b", GetBValue(DefaultTextSettings.textShadowColor), pathStr) & 0xFF;
		textConfig.textShadowColor = RGB(textShadowR, textShadowG, textShadowB);
		textConfig.drawOverMS2 = GetPrivateProfileIntA("config", "draw_over_ms2", DefaultTextSettings.drawOverMS2, pathStr);

		console = GetPrivateProfileIntA("debug", "show_console", DefaultEnableConsole, pathStr);
		// Also enable console if "debug" file exists
		console |= fs::is_regular_file(gamePath / "debug");
		maxLogLevel = GetPrivateProfileIntA("debug", "log_level", DefaultLogLevel, pathStr);
		if (maxLogLevel < 0)
			maxLogLevel = 0;
		else if (maxLogLevel > 4)
			maxLogLevel = 4;
		ignoreUUID = GetPrivateProfileIntA("debug", "ignore_uuid_mismatch", DefaultIgnoreUUID, pathStr);
	}
}
} // end namespace csmulti
