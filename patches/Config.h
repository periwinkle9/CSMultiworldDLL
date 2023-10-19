#pragma once
class Config
{
	unsigned short port;
	bool console;
	bool enable60fps;
	int maxLogLevel;
public:
	Config();
	void load(const char* name);

	unsigned short serverPort() const { return port; }
	bool enableConsole() const { return console; }
	bool use60fps() const { return enable60fps; }
	int logLevel() const { return maxLogLevel; }
};

extern Config config;
