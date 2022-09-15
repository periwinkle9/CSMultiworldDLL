#pragma once

namespace csvanilla
{

// Functions
const auto InitTriangleTable = reinterpret_cast<void(*)(void)>(0x4257F0);
const auto GetSin = reinterpret_cast<int(*)(unsigned char)>(0x4258B0);
const auto GetCos = reinterpret_cast<int(*)(unsigned char)>(0x4258C0);
const auto GetArktan = reinterpret_cast<unsigned char(*)(int, int)>(0x4258E0);

// Global variables
static auto& gSin = *reinterpret_cast<int(*)[0x100]>(0x4A5B48);
static auto& gTan = *reinterpret_cast<short(*)[0x21]>(0x4A5F48);

}
