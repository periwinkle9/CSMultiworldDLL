#pragma once

namespace csmulti
{
class Config
{
public:
	struct TextSettings
	{
		int textX;
		int textY;
		int gitXoff;
		int gitYoff;
		unsigned long textColor;
		unsigned long textShadowColor;
		bool drawOverMS2;
	};
private:
	unsigned short port;
	bool console;
	bool enable60fps;
	bool ignoreUUID;
	int maxLogLevel;
	TextSettings textConfig;
public:
	Config();
	void load(const char* name);

	unsigned short serverPort() const { return port; }
	bool enableConsole() const { return console; }
	bool use60fps() const { return enable60fps; }
	bool ignoreUUIDMismatch() const { return ignoreUUID; }
	int logLevel() const { return maxLogLevel; }
	const TextSettings& textSettings() const { return textConfig; }
};
} // end namespace csmulti
