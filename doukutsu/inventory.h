#pragma once

#include "types.h"

namespace csvanilla
{

// Structures
struct ARMSDATA
{
	int code; // Weapon ID
	int level; // Current level (1-3)
	int exp; // Current exp
	int max_num; // Max ammo
	int num; // Current ammo
};
struct ITEMDATA
{
	int code; // Item ID
};
struct ARMSLEVELTABLE
{
	int exp[3];
};

// Functions
const auto ClearArmsData = reinterpret_cast<void(*)(void)>(0x401000);
const auto ClearItemData = reinterpret_cast<void(*)(void)>(0x401030);
const auto AddArmsData = reinterpret_cast<BOOL(*)(int, int)>(0x401050);
const auto SubArmsData = reinterpret_cast<BOOL(*)(int)>(0x401160);
const auto TradeArms = reinterpret_cast<BOOL(*)(int, int, int)>(0x401220);
const auto AddItemData = reinterpret_cast<BOOL(*)(int)>(0x4012D0);
const auto SubItemData = reinterpret_cast<BOOL(*)(int)>(0x401330);
const auto MoveCampCursor = reinterpret_cast<void(*)(void)>(0x4013C0);
const auto PutCampObject = reinterpret_cast<void(*)(void)>(0x4016F0);
const auto CampLoop = reinterpret_cast<int(*)(void)>(0x401D10);
const auto CheckItem = reinterpret_cast<BOOL(*)(int)>(0x401F20);
const auto CheckArms = reinterpret_cast<BOOL(*)(int)>(0x401F60);
const auto UseArmsEnergy = reinterpret_cast<BOOL(*)(int)>(0x401FA0);
const auto ChargeArmsEnergy = reinterpret_cast<BOOL(*)(int)>(0x402020);
const auto FullArmsEnergy = reinterpret_cast<void(*)(void)>(0x402090);
const auto RotationArms = reinterpret_cast<int(*)(void)>(0x4020E0);
const auto RotationArmsRev = reinterpret_cast<int(*)(void)>(0x402190);
const auto ChangeToFirstArms = reinterpret_cast<void(*)(void)>(0x402240);

// Global variables
static auto& gArmsData = *reinterpret_cast<ARMSDATA(*)[8]>(0x499BC8);
static auto& gItemData = *reinterpret_cast<ITEMDATA(*)[32]>(0x499B40);
static int& gSelectedArms = *reinterpret_cast<int*>(0x499C68);
//static int& gSelectedItem = *reinterpret_cast<int*>(0x499C6C); // Nobody will ever need this (probably)

static auto& gArmsLevelTable = *reinterpret_cast<ARMSLEVELTABLE(*)[14]>(0x493660);

}