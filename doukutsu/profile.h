#pragma once

#include "inventory.h"
#include "teleporter.h"

namespace csvanilla
{

// Structures
struct PROFILEDATA
{
	char code[8];
	int stage;
	int music;
	int x;
	int y;
	int direct;
	short max_life;
	short star;
	short life;
	short a;
	int select_arms;
	int select_item;
	int equip;
	int unit;
	int counter;
	ARMSDATA arms[8];
	ITEMDATA items[32];
	PERMITSTAGE permitstage[8];
	signed char permit_mapping[0x80];
	char FLAG[4];
	unsigned char flags[1000];
};

// Functions
const auto IsProfile = reinterpret_cast<BOOL(*)(void)>(0x41CFC0);
const auto SaveProfile = reinterpret_cast<BOOL(*)(const char*)>(0x41D040);
const auto LoadProfile = reinterpret_cast<BOOL(*)(const char*)>(0x41D260);
const auto InitializeGame = reinterpret_cast<BOOL(*)(void*)>(0x41D550); // Parameter is technically HWND

// Global variables
static const char*& gDefaultName = *reinterpret_cast<const char**>(0x4937A8);
static const char*& gProfileCode = *reinterpret_cast<const char**>(0x4937AC);

}