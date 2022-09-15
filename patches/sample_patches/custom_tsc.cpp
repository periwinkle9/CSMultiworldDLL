// An example of modding via patching new functionality into existing vanilla ASM
// (This sample hack implements <MIM into the game)

#include "custom_tsc.h"

#include "patch_utils.h"

#include "doukutsu/tsc.h"
#include "doukutsu/flags.h"
#include "doukutsu/player.h"
#include "doukutsu/npc.h"
#include "doukutsu/inventory.h"

namespace custom_tsc_cmds
{
using patcher::byte;
using patcher::dword;

// This function will be inserted at the beginning of the command parser
// (optimizing the start of the <END command), so it can also replace
// existing vanilla commands.
// It returns true if a command was processed (i.e. we should skip to the
// end of the if/else chain in the vanilla parser),
// or false otherwise (i.e. continue checking for vanilla commnds).
bool checkCustomCommands();
// We'll implement this function below, but first let's see how we implement the patcher:

void patchTSCHook()
{
	dword address = 0x4225ED; // Start of <END code
	// Extra bytes shaved off by optimizing the ASM
	patcher::writeNOPs(address, 19);
	address += 19;

	// Write the call to our function
	patcher::writeCall(address, checkCustomCommands);
	address += 5; // CALL instruction is 5 bytes

	// Handle the aftermath and replace the <END code that we clobbered (in ASM)
	byte patch2[] = {
		0x84, 0xC0,                         // TEST al, al (test return value)
		0x74, 0x05,                         // JE short 42260E (if false, jump to optimized <END code)
		0xE9, 0x99, 0x2C, 0x00, 0x00,       // JMP 4252A7 (jump to end of command parsing loop)
		// Optimized <END code (not like super optimized, but it doesn't need to be)
		0xA1, 0xD8, 0x5A, 0x4A, 0x00,       // MOV eax, dword ptr [4A5AD8] (gTS.data)
		0x03, 0x05, 0xE0, 0x5A, 0x4A, 0x00, // ADD eax, dword ptr [4A5AE0] (gTS.p_read)
		0x80, 0x78, 0x01, 0x45,             // CMP byte ptr [eax+1], 45 ('E')
		0x75, 0x47,                         // JNE SHORT 422666
		0x80, 0x78, 0x02, 0x4E,             // CMP byte ptr [eax+2], 4E ('N')
		0x75, 0x41,                         // JNE SHORT 422666
		0x80, 0x78, 0x03, 0x44              // CMP byte ptr [eax+3], 44 ('D')
	};
	patcher::patchBytes(address, patch2, sizeof patch2);
}

// This <MIM hack is completely untested and possibly doesn't even work. :quate:
// If anybody wants to try it out, let me know how it goes
void shiftNPCrect(csvanilla::NPCHAR* npc);
void patchMIM()
{
	// Just gonna copy the XML here
	const byte patchPutMyChar[] = {0xA1, 0x84, 0xE1, 0x49, 0x00, 0xC1, 0xE0, 0x05, 0x01, 0x45,
		0xF4, 0x01, 0x45, 0xFC, 0xEB, 0x0C};
	patcher::patchBytes(0x4154B8, patchPutMyChar, sizeof patchPutMyChar);

	// Patch NPC 150, 111, and 112
	constexpr dword addresses[] = {0x445D57, 0x43D003, 0x43D291};
	for (dword address : addresses)
	{
		// Just by coincidence, the same exact patch works for all 3 of these NPCs :)
		const byte patch1[] = {0xFF, 0x75, 0x08}; // PUSH DWORD PTR [EBP+08]
		patcher::patchBytes(address, patch1, sizeof patch1);
		address += sizeof patch1;
		patcher::writeCall(address, shiftNPCrect); // Call our function to shift the NPC rects
		address += 5; // A CALL is 5 bytes
		const byte patch2[] = {
			0x59,      // POP ECX
			0xEB, 0x1E // JMP (past the residual Mimiga Mask equip code)
		};
		patcher::patchBytes(address, patch2, sizeof patch2);
	}
}
void applyPatch()
{
	patchTSCHook();
	patchMIM();
}


// Below is stuff related to the custom commands
void shiftNPCrect(csvanilla::NPCHAR* npc)
{
	// Get flag used by the unobtrusive <MIM hack
	const dword* mimFlag = reinterpret_cast<dword*>(&csvanilla::gFlagNPC[996]);
	npc->rect.top += *mimFlag * 32;
	npc->rect.bottom += *mimFlag * 32;
}
void maxAllWeapons()
{
	using csvanilla::gArmsData;
	for (int i = 0; i < 8 && gArmsData[i].code != 0; ++i)
	{
		gArmsData[i].level = 3;
		gArmsData[i].exp = csvanilla::gArmsLevelTable[gArmsData[i].code].exp[2];
	}
}

bool checkCustomCommands()
{
	using namespace csvanilla;
	
	auto isCommand = [](const char* cmd)
	{
		const char* addr = &csvanilla::gTS.data[csvanilla::gTS.p_read];
		return addr[1] == cmd[0] && addr[2] == cmd[1] && addr[3] == cmd[2];
	};
	if (isCommand("MIM"))
	{
		dword* mimFlag = reinterpret_cast<dword*>(&gFlagNPC[996]);
		*mimFlag = GetTextScriptNo(gTS.p_read + 4);
		gTS.p_read += 8;
	}
	// Just screwing around here
	else if (isCommand("MAE")) // Max Arms Energy - set all weapons to L3 max
	{
		maxAllWeapons();
		gTS.p_read += 4;
	}
	else if (isCommand("ML+")) // You can override existing commands too
	{
		// Give double the HP :)
		int hp = GetTextScriptNo(gTS.p_read + 4);
		AddMaxLifeMyChar(hp * 2);
		gTS.p_read += 8;
	}
	else
		return false; // return false if not any of these commands

	return true;
}

}