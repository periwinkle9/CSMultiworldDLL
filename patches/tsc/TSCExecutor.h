#pragma once

#include <string>
#include <string_view>

class TSCExecutor
{
	static constexpr int TextSurfaceID = 34;
	std::string scriptBuffer; // Custom script that can be executed
	std::string_view currentScript; // Currently-executing script (can be either custom or vanilla script)
	std::string_view::const_iterator currentPos; // Current script read position
	enum class OperationMode { IDLE, RUNNING, WAITING, WAITUNTILSTANDING, FADE } mode;
	int wait;
	int item; // <GIT graphic
	int NUMnum; // Number storage for <NUM command
	std::string text; // Message box text
	bool activeTextbox;
	bool updateText;

	enum class CommandStatus { PROCESSNEXT, STOPPROCESSING, ISTEXT };
	CommandStatus processCommand();
	void processText();
	void writeTextToSurface() const;
	void clearText();
	void jumpEvent(int eventNum);
	void resetState();
public:
	TSCExecutor();

	void runScript(std::string script);
	void runEvent(int eventNum);

	void tick();
	void draw() const;

	bool isRunning() const { return mode != OperationMode::IDLE; }
};

extern TSCExecutor* secondaryTSCParser;

void initTSC2();
void endTSC2();
