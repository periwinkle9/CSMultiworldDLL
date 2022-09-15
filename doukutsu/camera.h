#pragma once

namespace csvanilla
{

// Structures
struct FRAME
{
	int x;
	int y;
	int* tgt_x;
	int* tgt_y;
	int wait;
	int quake;
	int quake2;
};

// Functions
const auto MoveFrame3 = reinterpret_cast<void(*)(void)>(0x40EE70);
const auto GetFramePosition = reinterpret_cast<void(*)(int*, int*)>(0x40F020);
const auto SetFramePosition = reinterpret_cast<void(*)(int, int)>(0x40F040);
const auto SetFrameMyChar = reinterpret_cast<void(*)(void)>(0x40F130);
const auto SetFrameTargetMyChar = reinterpret_cast<void(*)(int)>(0x40F220);
const auto SetFrameTargetNpChar = reinterpret_cast<void(*)(int, int)>(0x40F250);
const auto SetFrameTargetBoss = reinterpret_cast<void(*)(int, int)>(0x40F2D0);
const auto SetQuake = reinterpret_cast<void(*)(int)>(0x40F310);
const auto SetQuake2 = reinterpret_cast<void(*)(int)>(0x40F320);
const auto ResetQuake = reinterpret_cast<void(*)(void)>(0x40F330);

// Global variables
static FRAME& gFrame = *reinterpret_cast<FRAME*>(0x49E1C8);

}