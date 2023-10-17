#pragma once
class Config
{
	unsigned short port;
	bool reuseAddress;
	bool console;
	int maxLogLevel;
public:
	Config();
	void load(const char* name);

	unsigned short serverPort() const { return port; }
	bool allowReuseAddress() const { return reuseAddress; }
	bool enableConsole() const { return console; }
	int logLevel() const { return maxLogLevel; }
};

extern Config config;
