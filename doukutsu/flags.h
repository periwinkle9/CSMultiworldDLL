#pragma once

#include "types.h"

namespace csvanilla
{

// Functions
const auto InitFlags = reinterpret_cast<void(*)(void)>(0x40E850);
const auto InitSkipFlags = reinterpret_cast<void(*)(void)>(0x40E870);
const auto SetNPCFlag = reinterpret_cast<void(*)(int)>(0x40E890);
const auto CutNPCFlag = reinterpret_cast<void(*)(int)>(0x40E8E0);
const auto GetNPCFlag = reinterpret_cast<BOOL(*)(int)>(0x40E930);
const auto SetSkipFlag = reinterpret_cast<void(*)(int)>(0x40E970);
const auto CutSkipFlag = reinterpret_cast<void(*)(int)>(0x40E9C0);
const auto GetSkipFlag = reinterpret_cast<BOOL(*)(int)>(0x40EA10);

// Global variables
static auto& gFlagNPC = *reinterpret_cast<unsigned char(*)[1000]>(0x49DDA0);
static auto& gSkipFlag = *reinterpret_cast<unsigned char(*)[8]>(0x49DD98);

}