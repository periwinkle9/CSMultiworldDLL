#pragma once

namespace csvanilla
{

// Functions
const auto SetNoise = reinterpret_cast<void(*)(int, int)>(0x416CC0);
const auto CutNoise = reinterpret_cast<void(*)(void)>(0x416D40);
const auto ResetNoise = reinterpret_cast<void(*)(void)>(0x416D80);
const auto SleepNoise = reinterpret_cast<void(*)(void)>(0x416DF0);

// Global variables
static int& noise_no = *reinterpret_cast<int*>(0x49E6EC);
static unsigned int& noise_freq = *reinterpret_cast<unsigned int*>(0x49E6F0);

}