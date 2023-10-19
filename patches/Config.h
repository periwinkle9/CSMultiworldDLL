#pragma once
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
	int maxLogLevel;
	TextSettings textConfig;
public:
	Config();
	void load(const char* name);

	unsigned short serverPort() const { return port; }
	bool enableConsole() const { return console; }
	bool use60fps() const { return enable60fps; }
	int logLevel() const { return maxLogLevel; }
	const TextSettings& textSettings() const { return textConfig; }
};

extern Config config;
