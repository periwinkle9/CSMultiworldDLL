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
	wait{0}, item{0}, NUMnum{0}, text{}, activeTextbox{false}, updateText{false}
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
	text.clear();
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

void TSCExecutor::tick()
{
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
	// Stop when reaching a command
	while (textEndPos != currentScript.end() && *textEndPos != '<')
		++textEndPos;
	std::string newText{currentPos, textEndPos};
	std::replace(std::begin(newText), std::end(newText), '=', '\x95'); // Replace all '=' with bullet point
	text += newText; // TODO is this correct?
	currentPos = textEndPos;
	updateText = true;
}

void TSCExecutor::clearText()
{
	text.clear();
	updateText = true;
}

void TSCExecutor::writeTextToSurface() const
{
	// Clear out surface first, just to be safe
	const RECT TextSurfaceRect = {0, 0, 216, 48};
	csvanilla::CortBox2(&TextSurfaceRect, 0, TextSurfaceID);

	csvanilla::PutText2(0, 0, text.c_str(), RGB(0xFF, 0xFF, 0xFE), TextSurfaceID);
}

void TSCExecutor::jumpEvent(int eventNum)
{
	// Apparently <EVE clears the textbox
	clearText();
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
	// Nothing to draw if not running or no textbox
	if (mode == OperationMode::IDLE || !activeTextbox)
		return;
	// TODO adjust these rects...
	const RECT TextRect = {70, 80, 268, 144};
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
			csvanilla::PutBitmap3(&csvanilla::grcFull, 44, TextRect.top - 3, &armsRect, 12);
		}
		else
		{
			int itemID = item - 1000;
			RECT itemRect;
			itemRect.left = (itemID % 8) * 32;
			itemRect.top = (itemID / 8) * 16;
			itemRect.right = itemRect.left + 32;
			itemRect.bottom = itemRect.top + 16;
			csvanilla::PutBitmap3(&csvanilla::grcFull, 38, TextRect.top - 3, &itemRect, 8);
		}
	}
}
