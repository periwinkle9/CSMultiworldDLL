#pragma once

namespace csvanilla
{

// Structures
struct BACKDATA
{
	int flag;   // Unused
	int partsW; // Width
	int partsH; // Height
	int numX;   // Unused
	int numY;   // Unused
	int type;   // Background type
	int fx;     // Used to control the scrolling effect
};

// Functions
const auto InitBack = reinterpret_cast<int(*)(const char*, int)>(0x402270);
const auto ActBack = reinterpret_cast<void(*)(void)>(0x402370);
const auto PutBack = reinterpret_cast<void(*)(int, int)>(0x4023D0);
const auto PutFront = reinterpret_cast<void(*)(int, int)>(0x402830);

// Global variables
static BACKDATA& gBack = *reinterpret_cast<BACKDATA*>(0x499C74);
static int& gWaterY = *reinterpret_cast<int*>(0x499C90); // Global water level

}