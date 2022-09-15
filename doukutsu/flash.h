#pragma once

#include "types.h"

namespace csvanilla
{

// Structures
struct FLASH
{
	int mode;
	int act_no;
	BOOL flag;
	int cnt;
	int width;
	int x;
	int y;
	RECT rect1;
	RECT rect2;
};

// Functions
const auto InitFlash = reinterpret_cast<void(*)(void)>(0x40EA50);
const auto SetFlash = reinterpret_cast<void(*)(int, int, int)>(0x40EA70);
const auto ActFlash_Explosion = reinterpret_cast<void(*)(int, int)>(0x40EAC0);
const auto ActFlash_Flash = reinterpret_cast<void(*)(void)>(0x40ED20);
const auto ActFlash = reinterpret_cast<void(*)(int, int)>(0x40EDE0);
const auto PutFlash = reinterpret_cast<void(*)(void)>(0x40EE20);
const auto ResetFlash = reinterpret_cast<void(*)(void)>(0x40EE60);

// Global variables
static FLASH& flash = *reinterpret_cast<FLASH*>(0x49E188);
static unsigned long& gFlashColor = *reinterpret_cast<unsigned long*>(0x49E1C4);

}