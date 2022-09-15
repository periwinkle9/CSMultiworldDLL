#pragma once

#include "types.h"

namespace csvanilla
{

// Structures
struct CREDITSCRIPT
{
	int size;
	char* pData;
	int offset;
	int wait;
	int mode;
	int start_x;
};
struct STRIPPER
{
	int flag;
	int x;
	int y;
	int cast;
	char str[0x40];
};
struct ILLUST
{
	int act_no;
	int x;
};
struct SPRITESTRUCT
{
	int x;
	int y;
};

// Functions
const auto ActionStripper = reinterpret_cast<void(*)(void)>(0x40CF90);
const auto PutStripper = reinterpret_cast<void(*)(void)>(0x40D010);
const auto SetStripper = reinterpret_cast<void(*)(int, int, const char*, int)>(0x40D150);
const auto RestoreStripper = reinterpret_cast<void(*)(void)>(0x40D240);
const auto ActionIllust = reinterpret_cast<void(*)(void)>(0x40D2D0);
const auto PutIllust = reinterpret_cast<void(*)(void)>(0x40D350);
const auto ReloadIllust = reinterpret_cast<void(*)(int)>(0x40D3A0);
const auto InitCreditScript = reinterpret_cast<void(*)(void)>(0x40D3E0);
const auto ReleaseCreditScript = reinterpret_cast<void(*)(void)>(0x40D410);
const auto StartCreditScript = reinterpret_cast<BOOL(*)(void)>(0x40D440);
const auto ActionCredit = reinterpret_cast<void(*)(void)>(0x40D5C0);
const auto ActionCredit_Read = reinterpret_cast<void(*)(void)>(0x40D620);
const auto GetScriptNumber = reinterpret_cast<int(*)(const char*)>(0x40DB00);
const auto SetCreditIllust = reinterpret_cast<void(*)(int)>(0x40DB40);
const auto CutCreditIllust = reinterpret_cast<void(*)(void)>(0x40DB60);

}