#include "TSCExecutor.h"
#include <cstdint>
#include <iterator>
#include <Windows.h>
#include "doukutsu/audio.h"
#include "doukutsu/boss.h"
#include "doukutsu/camera.h"
#include "doukutsu/fade.h"
#include "doukutsu/flags.h"
#include "doukutsu/flash.h"
#include "doukutsu/inventory.h"
#include "doukutsu/map.h"
#include "doukutsu/npc.h"
#include "doukutsu/organya.h"
#include "doukutsu/player.h"
#include "doukutsu/profile.h"
#include "doukutsu/sound.h"
#include "doukutsu/teleporter.h"

namespace
{

std::uint32_t cmd2int(std::string_view cmd)
{
	std::uint32_t val = 0;
	for (int i = 0; i < 4; ++i)
		val |= (static_cast<unsigned char>(cmd[i]) << (i * 8));
	return val;
}

int tscval2int(std::string_view str)
{
	constexpr int PlaceValues[] = {1000, 100, 10, 1};
	int val = 0;
	for (int i = 0; i < 4; ++i)
		val += (str[i] - '0') * PlaceValues[i];
	return val;
}

// This might actually not be needed in later C++ versions because cmd2int could be constexpr?
// But Visual Studio doesn't like that in Debug builds at least, so let's keep this template around for now
template <unsigned char A, unsigned char B, unsigned char C>
struct TSCcmd
{
	static constexpr std::uint32_t value = '<' | (A << 8) | (B << 16) | (C << 24);
};

}

auto TSCExecutor::processCommand() -> CommandStatus
{
	if (currentPos == currentScript.end())
	{
		mode = OperationMode::IDLE;
		return CommandStatus::STOPPROCESSING;
	}
	// Handle newline
	if (*currentPos == '\r' || *currentPos == '\n')
	{
		++currentPos;
		if (currentPos != currentScript.cend() && *currentPos == '\n')
			++currentPos;
		if (activeTextbox && !text.empty()) // Avoid leading newlines
		{
			text += "\r\n"; // Let's hope this actually works
			updateText = true;
		}
		return CommandStatus::PROCESSNEXT;
	}
	// Treat invalid commands as text
	if (std::distance(currentPos, currentScript.cend()) < 4 || *currentPos != '<')
		return CommandStatus::ISTEXT;

	std::string_view commandSubstr{currentPos, currentScript.cend()};

	auto hasArgs = [commandSubstr](unsigned int argCount)
	{
		return commandSubstr.size() >= argCount * 5 + 3;
	};
	auto getArg = [commandSubstr](int argNum)
	{
		return tscval2int(commandSubstr.substr(argNum * 5 - 1, 4));
	};

	bool keepProcessing = true;
	switch (cmd2int(commandSubstr))
	{
	case TSCcmd<'E', 'N', 'D'>::value:
		mode = OperationMode::IDLE;
		keepProcessing = false;
		break;
	case TSCcmd<'L', 'I', '+'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::AddLifeMyChar(getArg(1));
		currentPos += 8;
		break;
	case TSCcmd<'M','L','+'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::AddMaxLifeMyChar(getArg(1));
		currentPos += 8;
		break;
	case TSCcmd<'A','E','+'>::value:
		csvanilla::FullArmsEnergy();
		currentPos += 4;
		break;
	case TSCcmd<'I','T','+'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::PlaySoundObject(38, 1); // Play get item sound
		csvanilla::AddItemData(getArg(1));
		currentPos += 8;
		break;
	case TSCcmd<'I', 'T', '-'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::SubItemData(getArg(1));
		currentPos += 8;
		break;
	case TSCcmd<'E','Q','+'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::EquipItem(getArg(1), 1);
		currentPos += 8;
		break;
	case TSCcmd<'E','Q','-'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::EquipItem(getArg(1), 0);
		currentPos += 8;
		break;
	case TSCcmd<'A','M','+'>::value:
		if (!hasArgs(2))
			return CommandStatus::ISTEXT;
		NUMnum = getArg(2);
		csvanilla::PlaySoundObject(38, 1); // Play get item sound
		csvanilla::AddArmsData(getArg(1), NUMnum);
		currentPos += 13;
		break;
	case TSCcmd<'A','M','-'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::SubArmsData(getArg(1));
		currentPos += 8;
		break;
	case TSCcmd<'Z','A','M'>::value:
		csvanilla::ZeroArmsEnergy_All();
		currentPos += 4;
		break;
	case TSCcmd<'T','A','M'>::value:
		if (!hasArgs(3))
			return CommandStatus::ISTEXT;
		csvanilla::TradeArms(getArg(1), getArg(2), getArg(3));
		currentPos += 18;
		break;
	case TSCcmd<'P','S','+'>::value:
		if (!hasArgs(2))
			return CommandStatus::ISTEXT;
		csvanilla::AddPermitStage(getArg(1), getArg(2));
		currentPos += 13;
		break;
	case TSCcmd<'M','P','+'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::SetMapping(getArg(1));
		currentPos += 8;
		break;
	case TSCcmd<'U','N','I'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::ChangeMyUnit(getArg(1));
		currentPos += 8;
		break;
	case TSCcmd<'S','T','C'>::value:
		csvanilla::SaveTimeCounter();
		currentPos += 4;
		break;
	case TSCcmd<'T','R','A'>::value:
		if (!hasArgs(4))
			return CommandStatus::ISTEXT;
		if (!csvanilla::TransferStage(getArg(1), getArg(2), getArg(3), getArg(4)))
			MessageBoxA(NULL, "Failed to load stage!", "Error", MB_OK | MB_ICONEXCLAMATION);
		currentPos += 23;
		break;
	case TSCcmd<'M','O','V'>::value:
		if (!hasArgs(2))
			return CommandStatus::ISTEXT;
		csvanilla::SetMyCharPosition(getArg(1) * 0x2000, getArg(2) * 0x2000);
		currentPos += 13;
		break;
	case TSCcmd<'H','M','C'>::value:
		csvanilla::ShowMyChar(0);
		currentPos += 4;
		break;
	case TSCcmd<'S','M','C'>::value:
		csvanilla::ShowMyChar(1);
		currentPos += 4;
		break;
	case TSCcmd<'F', 'L', '+'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::SetNPCFlag(getArg(1));
		currentPos += 8;
		break;
	case TSCcmd<'F', 'L', '-'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::CutNPCFlag(getArg(1));
		currentPos += 8;
		break;
	case TSCcmd<'S', 'K', '+'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::SetSkipFlag(getArg(1));
		currentPos += 8;
		break;
	case TSCcmd<'S', 'K', '-'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::CutSkipFlag(getArg(1));
		currentPos += 8;
		break;
	case TSCcmd<'N','O','D'>::value:
		// Instead of waiting for user input, wait for a set amount of time
		mode = OperationMode::WAITING;
		wait = 100;
		currentPos += 4;
		keepProcessing = false;
		break;
	case TSCcmd<'C','L','R'>::value:
		clearText();
		currentPos += 4;
		break;
	case TSCcmd<'M','S','G'>::value:
	case TSCcmd<'M','S','2'>::value:
	case TSCcmd<'M','S','3'>::value:
		clearText();
		activeTextbox = true;
		currentPos += 4;
		keepProcessing = false;
		break;
	case TSCcmd<'W','A','I'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		mode = OperationMode::WAITING;
		wait = getArg(1);
		currentPos += 8;
		keepProcessing = false;
		break;
	case TSCcmd<'W','A','S'>::value:
		mode = OperationMode::WAITUNTILSTANDING;
		currentPos += 4;
		keepProcessing = false;
		break;
	case TSCcmd<'C','L','O'>::value:
		activeTextbox = false;
		currentPos += 4;
		break;
	case TSCcmd<'E','V','E'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		jumpEvent(getArg(1));
		break;
	case TSCcmd<'Y','N','J'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		// Little hack to delay for a bit before automatically saying "no"
		if (++wait > 50)
		{
			wait = 0;
			jumpEvent(getArg(1));
		}
		else
			keepProcessing = false;
		break;
	case TSCcmd<'F','L','J'>::value:
		if (!hasArgs(2))
			return CommandStatus::ISTEXT;
		if (csvanilla::GetNPCFlag(getArg(1)))
			jumpEvent(getArg(2));
		else
			currentPos += 13;
		break;
	case TSCcmd<'S','K','J'>::value:
		if (!hasArgs(2))
			return CommandStatus::ISTEXT;
		if (csvanilla::GetSkipFlag(getArg(1)))
			jumpEvent(getArg(2));
		else
			currentPos += 13;
		break;
	case TSCcmd<'I','T','J'>::value:
		if (!hasArgs(2))
			return CommandStatus::ISTEXT;
		if (csvanilla::CheckItem(getArg(1)))
			jumpEvent(getArg(2));
		else
			currentPos += 13;
		break;
	case TSCcmd<'A','M','J'>::value:
		if (!hasArgs(2))
			return CommandStatus::ISTEXT;
		if (csvanilla::CheckArms(getArg(1)))
			jumpEvent(getArg(2));
		else
			currentPos += 13;
		break;
	case TSCcmd<'U','N','J'>::value:
		if (!hasArgs(2))
			return CommandStatus::ISTEXT;
		if (csvanilla::GetUnitMyChar() == getArg(1))
			jumpEvent(getArg(2));
		else
			currentPos += 13;
		break;
	case TSCcmd<'E','C','J'>::value:
		if (!hasArgs(2))
			return CommandStatus::ISTEXT;
		if (csvanilla::GetNpCharAlive(getArg(1)))
			jumpEvent(getArg(2));
		else
			currentPos += 13;
		break;
	case TSCcmd<'N','C','J'>::value:
		if (!hasArgs(2))
			return CommandStatus::ISTEXT;
		if (csvanilla::IsNpCharCode(getArg(1)))
			jumpEvent(getArg(2));
		else
			currentPos += 13;
		break;
	case TSCcmd<'M','P','J'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		if (csvanilla::IsMapping())
			jumpEvent(getArg(1));
		else
			currentPos += 8;
		break;
	case TSCcmd<'S','S','S'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::SetNoise(1, getArg(1));
		currentPos += 8;
		break;
	case TSCcmd<'C','S','S'>::value:
	case TSCcmd<'C','P','S'>::value:
		csvanilla::CutNoise();
		currentPos += 4;
		break;
	case TSCcmd<'S','P','S'>::value:
		csvanilla::SetNoise(2, 0);
		currentPos += 4;
		break;
	case TSCcmd<'Q','U','A'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::SetQuake(getArg(1));
		currentPos += 8;
		break;
	case TSCcmd<'F','L','A'>::value:
		csvanilla::SetFlash(0, 0, 2);
		currentPos += 4;
		break;
	case TSCcmd<'F','A','I'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::StartFadeIn(getArg(1));
		mode = OperationMode::FADE;
		currentPos += 8;
		keepProcessing = false;
		break;
	case TSCcmd<'F','A','O'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::StartFadeOut(getArg(1));
		mode = OperationMode::FADE;
		currentPos += 8;
		keepProcessing = false;
		break;
	case TSCcmd<'M','N','A'>::value:
		csvanilla::StartMapName();
		currentPos += 4;
		break;
	case TSCcmd<'F','O','M'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::SetFrameTargetMyChar(getArg(1));
		currentPos += 8;
		break;
	case TSCcmd<'F','O','N'>::value:
		if (!hasArgs(2))
			return CommandStatus::ISTEXT;
		csvanilla::SetFrameTargetNpChar(getArg(1), getArg(2));
		currentPos += 13;
		break;
	case TSCcmd<'F','O','B'>::value:
		if (!hasArgs(2))
			return CommandStatus::ISTEXT;
		csvanilla::SetFrameTargetBoss(getArg(1), getArg(2));
		currentPos += 13;
		break;
	case TSCcmd<'S','O','U'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::PlaySoundObject(getArg(1), 1);
		currentPos += 8;
		break;
	case TSCcmd<'C','M','U'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::ChangeMusic(getArg(1));
		currentPos += 8;
		break;
	case TSCcmd<'F','M','U'>::value:
		csvanilla::SetOrganyaFadeout();
		currentPos += 4;
		break;
	case TSCcmd<'R','M','U'>::value:
		csvanilla::ReCallMusic();
		currentPos += 4;
		break;
	case TSCcmd<'D','N','P'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::DeleteNpCharEvent(getArg(1));
		currentPos += 8;
		break;
	case TSCcmd<'D','N','A'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::DeleteNpCharCode(getArg(1), TRUE);
		currentPos += 8;
		break;
	case TSCcmd<'B','O','A'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::SetBossCharActNo(getArg(1));
		currentPos += 8;
		break;
	case TSCcmd<'C','N','P'>::value:
		if (!hasArgs(3))
			return CommandStatus::ISTEXT;
		csvanilla::ChangeNpCharByEvent(getArg(1), getArg(2), getArg(3));
		currentPos += 18;
		break;
	case TSCcmd<'A','N','P'>::value:
		if (!hasArgs(3))
			return CommandStatus::ISTEXT;
		csvanilla::SetNpCharActionNo(getArg(1), getArg(2), getArg(3));
		currentPos += 18;
		break;
	case TSCcmd<'I','N','P'>::value:
		if (!hasArgs(3))
			return CommandStatus::ISTEXT;
		csvanilla::ChangeCheckableNpCharByEvent(getArg(1), getArg(2), getArg(3));
		currentPos += 18;
		break;
	case TSCcmd<'S','N','P'>::value:
		if (!hasArgs(4))
			return CommandStatus::ISTEXT;
		csvanilla::SetNpChar(getArg(1), getArg(2) * 0x2000, getArg(3) * 0x2000, 0, 0, getArg(4), nullptr, 0x100);
		currentPos += 23;
		break;
	case TSCcmd<'M','N','P'>::value:
		if (!hasArgs(4))
			return CommandStatus::ISTEXT;
		csvanilla::MoveNpChar(getArg(1), getArg(2) * 0x2000, getArg(3) * 0x2000, getArg(4));
		currentPos += 23;
		break;
	case TSCcmd<'S','M','P'>::value:
		if (!hasArgs(2))
			return CommandStatus::ISTEXT;
		csvanilla::ShiftMapParts(getArg(1), getArg(2));
		currentPos += 13;
		break;
	case TSCcmd<'C','M','P'>::value:
		if (!hasArgs(3))
			return CommandStatus::ISTEXT;
		csvanilla::ChangeMapParts(getArg(1), getArg(2), getArg(3));
		currentPos += 18;
		break;
	case TSCcmd<'B','S','L'>::value:
	{
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		int arg = getArg(1);
		if (arg != 0)
			csvanilla::StartBossLife(arg);
		else
			csvanilla::StartBossLife2();
		currentPos += 8;
		break;
	}
	case TSCcmd<'M','Y','D'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::SetMyCharDirect(getArg(1));
		currentPos += 8;
		break;
	case TSCcmd<'M','Y','B'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		csvanilla::BackStepMyChar(getArg(1));
		currentPos += 8;
		break;
	case TSCcmd<'M','M','0'>::value:
		csvanilla::ZeroMyCharXMove();
		currentPos += 4;
		break;
	case TSCcmd<'S','V','P'>::value:
		csvanilla::SaveProfile(nullptr);
		currentPos += 4;
		break;
	case TSCcmd<'G','I','T'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		item = getArg(1);
		currentPos += 8;
		break;
	case TSCcmd<'N','U','M'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		text += std::to_string(NUMnum);
		updateText = true;
		currentPos += 8;
		break;
	case TSCcmd<'E','S','C'>::value:
		// Treat this as <END
		mode = OperationMode::IDLE;
		keepProcessing = false;
		break;
	// Recognize these commands, but ignore them
	case TSCcmd<'K','E','Y'>::value:
	case TSCcmd<'P','R','I'>::value:
	case TSCcmd<'F','R','E'>::value:
	case TSCcmd<'T','U','R'>::value: // Text is always <TUR
	case TSCcmd<'S','A','T'>::value:
	case TSCcmd<'C','A','T'>::value:
	case TSCcmd<'M','L','P'>::value: // Is this something worth implementing?
	case TSCcmd<'S','L','P'>::value: // Same with this
	case TSCcmd<'I','N','I'>::value: // Or this
	case TSCcmd<'L','D','P'>::value: // Or this
	case TSCcmd<'C','R','E'>::value:
	case TSCcmd<'C','I','L'>::value:
		currentPos += 4;
		break;
	case TSCcmd<'F','A','C'>::value: // Not supported
	case TSCcmd<'S','I','L'>::value:
	case TSCcmd<'X','X','1'>::value:
		if (!hasArgs(1))
			return CommandStatus::ISTEXT;
		currentPos += 8;
		break;
	default:
		return CommandStatus::ISTEXT;
	}

	return keepProcessing ? CommandStatus::PROCESSNEXT : CommandStatus::STOPPROCESSING;
}
