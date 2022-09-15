#pragma once

#include "types.h"

namespace csvanilla
{

// Structures
struct FADEPARAMETER
{
	int mode;
	BOOL bMask;
	int count;
	signed char ani_no[15][20];
	signed char flag[15][20];
	signed char dir;
};

// Functions
const auto InitFade = reinterpret_cast<void(*)(void)>(0x40DE60);
const auto SetFadeMask = reinterpret_cast<void(*)(void)>(0x40DE90);
const auto ClearFade = reinterpret_cast<void(*)(void)>(0x40DEA0);
const auto StartFadeOut = reinterpret_cast<void(*)(int)>(0x40DEC0);
const auto StartFadeIn = reinterpret_cast<void(*)(int)>(0x40DF50);
const auto ProcFade = reinterpret_cast<void(*)(void)>(0x40DFE0);
const auto PutFade = reinterpret_cast<void(*)(void)>(0x40E770);
const auto GetFadeActive = reinterpret_cast<BOOL(*)(void)>(0x40E830);

// Global variables
static FADEPARAMETER& gFade = *reinterpret_cast<FADEPARAMETER*>(0x49DB30);
static unsigned long& mask_color = *reinterpret_cast<unsigned long*>(0x49DB28);

}