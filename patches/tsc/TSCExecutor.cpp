#include "TSCExecutor.h"
#include <cstdint>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <string>
#include <format>
#include <algorithm>
#include <utility>
#include "../Multiworld.h"
#include "doukutsu/draw.h"
#include "doukutsu/fade.h"
#include "doukutsu/player.h"
#include "doukutsu/tsc.h"

namespace
{
const auto& config = csmulti::Multiworld::getInstance().config();
auto& logger = csmulti::Multiworld::getInstance().logger();
}

const RECT TextSurfaceRect = {0, 0, 216, 48};

TSCExecutor::TSCExecutor() : scriptBuffer{}, currentScript{}, currentPos{}, mode{OperationMode::IDLE},
	wait{0}, item{0}, NUMnum{0}, textLines{}, currentLine{0}, activeTextbox{false}, updateText{false}
{
	// Create text surface using unused surface ID
	// (This way we won't have to clean it up ourselves)
	if (!csvanilla::MakeSurface_Generic(TextSurfaceRect.right, TextSurfaceRect.bottom, TextSurfaceID, FALSE))
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
	logger.logDebug("Now executing script: " + scriptBuffer);
}

void TSCExecutor::runEvent(int eventNum)
{
	// Reset state (jumpEvent will reset the rest)
	item = 0;
	activeTextbox = false;
	jumpEvent(eventNum);
}

void TSCExecutor::endEvent()
{
	if (mode != OperationMode::IDLE)
		logger.logInfo("Stopping parallel TSC parser");
	resetState();
	mode = OperationMode::IDLE;
}

static bool isMS2Active()
{
	using csvanilla::gTS;
	return gTS.mode != 0 && (gTS.flags & 0x21) == 0x21;
}

void TSCExecutor::tick()
{
	// Pause when <MS2/<MS3 is active to avoid drawing over it
	if (activeTextbox && !config.textSettings().drawOverMS2 && isMS2Active())
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
	{
		textLines[currentLine] += newText;
		if (textLines[currentLine].size() > 43) // Empirically-observed max text line length without cutting off
			logger.logWarning("Text line too long; it will be cut off");
	}
	else
		logger.logWarning(std::format("Too many text lines (current line = {}), ignoring all text past the 3rd line", currentLine));
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
	csvanilla::CortBox2(&TextSurfaceRect, 0, TextSurfaceID);

	for (unsigned i = 0; i < textLines.size(); ++i)
		if (!textLines[i].empty())
		{
			// Draw text with shadow
			csvanilla::PutText2(0, i * 14 + 1, textLines[i].c_str(), config.textSettings().textShadowColor, TextSurfaceID);
			csvanilla::PutText2(0, i * 14, textLines[i].c_str(), config.textSettings().textColor, TextSurfaceID);
		}
}

void TSCExecutor::jumpEvent(int eventNum)
{
	// Apparently <EVE clears the textbox
	clearText();
	wait = 0; // I don't know if resetting this is necessary, but let's do it anyways just to be safe
	// Look for event in vanilla script
	const int TSC_BUFFER_SIZE = 0x5000; // Vanilla TSC buffer allocation size
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
		logger.logDebug(std::format("Now executing event {}", eventNum));
	}
	else
	{
		mode = OperationMode::IDLE;
		logger.logWarning(std::format("Event {} not found, stopping parallel TSC parser", eventNum));
	}
}

void TSCExecutor::draw() const
{
	// Nothing to draw if not running, no textbox, or <MS2/<MS3 is active in the main parser
	if (mode == OperationMode::IDLE || !activeTextbox || (!config.textSettings().drawOverMS2 && isMS2Active()))
		return;
	// X and Y coordinates of text and <GIT graphic
	const int TextX = config.textSettings().textX;
	const int TextY = config.textSettings().textY;
	const int GITX = TextX + config.textSettings().gitXoff;
	const int GITY = TextY + config.textSettings().gitYoff;
	const RECT TextRect = {TextX, TextY, TextX + TextSurfaceRect.right, TextY + TextSurfaceRect.bottom};
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
