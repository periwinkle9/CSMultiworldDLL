#pragma once
class Config
{
	unsigned short port;
	bool console;
	int maxLogLevel;
public:
	Config();
	void load(const char* name);

	unsigned short serverPort() const { return port; }
	bool enableConsole() const { return console; }
	int logLevel() const { return maxLogLevel; }
};

extern Config config;
