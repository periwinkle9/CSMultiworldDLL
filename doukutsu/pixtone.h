#pragma once

#include "types.h"

namespace csvanilla
{

// Structures
struct PIXOSCILLATOR
{
	int model;
	double num;
	int top;
	int offset;
};
struct PIXTONEPARAMETER
{
	int use;
	int size;
	PIXOSCILLATOR oMain;
	PIXOSCILLATOR oPitch;
	PIXOSCILLATOR oVolume;
	int initial;
	int pointAx;
	int pointAy;
	int pointBx;
	int pointBy;
	int pointCx;
	int pointCy;
};

// Functions
const auto MakeWaveTables = reinterpret_cast<void(*)(void)>(0x41C8F0);
const auto MakePixelWaveData = reinterpret_cast<BOOL(*)(const PIXTONEPARAMETER*, unsigned char)>(0x41CB10);
const auto MakePixToneObject = reinterpret_cast<int(*)(const PIXTONEPARAMETER*, int, int)>(0x4207E0);

// Global variables
static auto& gPtpTable = *reinterpret_cast<PIXTONEPARAMETER(*)[139]>(0x48F940);
static auto& gWaveModelTable = *reinterpret_cast<signed char(*)[6][0x100]>(0x4A4F00);

}