#include "TSCExecutor.h"
#include <cstdint>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <utility>
#include "doukutsu/draw.h"
#include "doukutsu/fade.h"
#include "doukutsu/player.h"
#include "doukutsu/tsc.h"

TSCExecutor* secondaryTSCParser;

void initTSC2()
{
	secondaryTSCParser = new TSCExecutor;
}
void endTSC2()
{
	delete secondaryTSCParser;
	secondaryTSCParser = nullptr;
}

TSCExecutor::TSCExecutor() : scriptBuffer{}, currentScript{}, currentPos{}, mode{OperationMode::IDLE},
	wait{0}, item{0}, NUMnum{0}, textLines{}, currentLine{0}, activeTextbox{false}, updateText{false}
{
	// Create text surface using unused surface ID
	// (This way we won't have to clean it up ourselves)
	if (!csvanilla::MakeSurface_Generic(216, 48, TextSurfaceID, FALSE))
		throw std::runtime_error("Couldn't create text surface");
}

void TSCExecutor::resetState()
{
	wait = 0;
	item = 0;
	NUMnum = 0;
	for (std::string& line: textLines)
		line.clear();
	currentLine = 0;
	activeTextbox = false;
	updateText = false;
}

void TSCExecutor::runScript(std::string script)
{
	resetState();
	scriptBuffer = std::move(script);
	currentScript = scriptBuffer;
	currentPos = std::begin(currentScript);
	mode = OperationMode::RUNNING;
}

void TSCExecutor::runEvent(int eventNum)
{
	// Reset state (jumpEvent will reset the rest)
	item = 0;
	activeTextbox = false;
	jumpEvent(eventNum);
}

static bool isMS2Active()
{
	using csvanilla::gTS;
	return gTS.mode != 0 && (gTS.flags & 0x21) == 0x21;
}

void TSCExecutor::tick()
{
	// Pause when <MS2/<MS3 is active to avoid drawing over it
	if (activeTextbox && isMS2Active())
		return;

	switch (mode)
	{
	case OperationMode::IDLE:
		return;
	case OperationMode::WAITING:
		--wait;
		if (wait <= 0)
		{
			wait = 0;
			mode = OperationMode::RUNNING;
		}
		break;
	case OperationMode::RUNNING:
	{
		bool exit = false;
		updateText = false;
		while (!exit)
		{
			switch (processCommand())
			{
			case CommandStatus::ISTEXT:
				processText();
				// Fallthrough
			case CommandStatus::STOPPROCESSING:
				exit = true;
				// Fallthrough
			case CommandStatus::PROCESSNEXT:
				break;
			}
		}
		if (updateText)
			writeTextToSurface();
		break;
	}
	case OperationMode::WAITUNTILSTANDING:
		if ((csvanilla::gMC.flag & 8) != 0)
			mode = OperationMode::RUNNING;
		break;
	case OperationMode::FADE:
		if (!csvanilla::GetFadeActive())
			mode = OperationMode::RUNNING;
		break;
	}
}

void TSCExecutor::processText()
{
	if (currentPos == currentScript.end())
		return;
	std::size_t size = 1;
	auto textEndPos = std::next(currentPos);
	// Stop when reaching a command or newline
	while (textEndPos != currentScript.end() && *textEndPos != '<' && *textEndPos != '\r' && *textEndPos != '\n')
		++textEndPos;
	std::string newText{currentPos, textEndPos};
	std::replace(std::begin(newText), std::end(newText), '=', '\x95'); // Replace all '=' with bullet point
	if (currentLine < textLines.size())
		textLines[currentLine] += newText;
	currentPos = textEndPos;
	updateText = true;
}

void TSCExecutor::clearText()
{
	for (std::string& line: textLines)
		line.clear();
	currentLine = 0;
	updateText = true;
}

void TSCExecutor::writeTextToSurface() const
{
	// Clear out surface first, just to be safe
	const RECT TextSurfaceRect = {0, 0, 216, 48};
	csvanilla::CortBox2(&TextSurfaceRect, 0, TextSurfaceID);

	for (unsigned i = 0; i < textLines.size(); ++i)
		if (!textLines[i].empty())
		{
			csvanilla::PutText2(0, i * 14 + 1, textLines[i].c_str(), RGB(0x11, 0x00, 0x22), TextSurfaceID);
			csvanilla::PutText2(0, i * 14, textLines[i].c_str(), RGB(0xFF, 0xFF, 0xFE), TextSurfaceID);
		}
}

void TSCExecutor::jumpEvent(int eventNum)
{
	wait = 0; // I don't know if resetting this is necessary, but let's do it anyways just to be safe
	// Look for event in vanilla script
	const int TSC_BUFFER_SIZE = 0x5000;
	const char* const tscBuffer = csvanilla::gTS.data;
	const char* eventStart = tscBuffer;
	bool found = false;
	while (!found)
	{
		eventStart = std::find(eventStart, tscBuffer + TSC_BUFFER_SIZE, '#');
		if (eventStart == tscBuffer + TSC_BUFFER_SIZE) // Not found
			break;
		++eventStart; // Advance to event number
		int event = csvanilla::GetTextScriptNo(eventStart - tscBuffer);
		if (event == eventNum)
			found = true;
		else if (event > eventNum) // Not found
			break;
	}

	if (found)
	{
		// Advance to next line
		while (eventStart != tscBuffer + TSC_BUFFER_SIZE && *eventStart != '\n')
			++eventStart;
		// Vanilla TSC assumes CRLF line endings, so let's do the same
		++eventStart;
		// Look for start of next event
		const char* nextEvent = std::find(eventStart, tscBuffer + TSC_BUFFER_SIZE, '#');
		// Set script state
		currentScript = std::string_view{eventStart, nextEvent};
		currentPos = std::begin(currentScript);
		mode = OperationMode::RUNNING;
	}
	else
		mode = OperationMode::IDLE;
}

void TSCExecutor::draw() const
{
	// Nothing to draw if not running, no textbox, or <MS2/<MS3 is active in the main parser
	if (mode == OperationMode::IDLE || !activeTextbox || isMS2Active())
		return;
	// TODO adjust these rects...
	constexpr int TextX = 84;
	constexpr int TextY = 33;
	constexpr int GITX = TextX + 82;
	constexpr int GITY = TextY - 18;
	const RECT TextRect = {TextX, TextY, TextX + 198, TextY + 64};
	const RECT TextSurfaceRect = {0, 0, 216, 48};
	// Draw text
	csvanilla::PutBitmap3(&TextRect, TextRect.left, TextRect.top, &TextSurfaceRect, TextSurfaceID);
	// Draw GIT
	if (item != 0)
	{
		if (item < 1000)
		{
			RECT armsRect;
			armsRect.left = (item % 16) * 16;
			armsRect.top = (item / 16) * 16;
			armsRect.right = armsRect.left + 16;
			armsRect.bottom = armsRect.top + 16;
			// Let's try drawing it here and see how that looks
			csvanilla::PutBitmap3(&csvanilla::grcFull, GITX, GITY, &armsRect, 12);
		}
		else
		{
			int itemID = item - 1000;
			RECT itemRect;
			itemRect.left = (itemID % 8) * 32;
			itemRect.top = (itemID / 8) * 16;
			itemRect.right = itemRect.left + 32;
			itemRect.bottom = itemRect.top + 16;
			csvanilla::PutBitmap3(&csvanilla::grcFull, GITX - 6, GITY, &itemRect, 8);
		}
	}
}
