#pragma once

#include "types.h"

namespace csvanilla
{

// Structures
struct RANGE_CHAR
{
	unsigned char front;
	unsigned char top;
	unsigned char back;
	unsigned char bottom;
};

struct NPCTABLE
{
	unsigned short bits;
	unsigned short life;
	unsigned char surf;
	unsigned char hit_voice;
	unsigned char destroy_voice;
	unsigned char size;
	int exp;
	int damage;
	RANGE_CHAR hit;
	RANGE_CHAR view;
};
typedef void(*NPCFUNCTION)(struct NPCHAR*);

// Functions
const auto LoadNpcTable = reinterpret_cast<BOOL(*)(const char*)>(0x472400);
const auto ReleaseNpcTable = reinterpret_cast<void(*)(void)>(0x472710);

// Global variables
static NPCTABLE*& gNpcTable = *reinterpret_cast<NPCTABLE**>(0x4BBA34);
static auto& gpNpcFuncTbl = *reinterpret_cast<NPCFUNCTION(*)[361]>(0x498548);

}