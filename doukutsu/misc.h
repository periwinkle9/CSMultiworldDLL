#pragma once

#include "types.h"

typedef struct _SYSTEMTIME SYSTEMTIME;

namespace csvanilla
{

// These void* parameters are technically HWND, but I'm not #include-ing Windows.h just for that typedef
const auto Scene_DownIsland = reinterpret_cast<void(*)(void*, int)>(0x40DB70);
const auto Call_Escape = reinterpret_cast<int(*)(void*)>(0x40DD70);
const auto Game = reinterpret_cast<BOOL(*)(void*)>(0x40F5F0);
const auto ModeOpening = reinterpret_cast<int(*)(void*)>(0x40F730);
const auto ModeTitle = reinterpret_cast<int(*)(void*)>(0x40F9B0);
const auto ModeAction = reinterpret_cast<int(*)(void*)>(0x410400);

const auto Random = reinterpret_cast<int(*)(int, int)>(0x40F350);
const auto PutNumber4 = reinterpret_cast<int(*)(int, int, int, BOOL)>(0x40F380);

const auto GetCompileDate = reinterpret_cast<void(*)(int*, int*, int*)>(0x4108B0);
const auto GetCompileVersion = reinterpret_cast<BOOL(*)(int*, int*, int*, int*)>(0x410990);
const auto OpenSoundVolume = reinterpret_cast<BOOL(*)(void*)>(0x410AB0); // Technically an HWND parameter
const auto DeleteLog = reinterpret_cast<void(*)(void)>(0x410BC0);
const auto WriteLog = reinterpret_cast<BOOL(*)(const char*, int, int, int)>(0x410C10);
const auto GetDateLimit = reinterpret_cast<int(*)(SYSTEMTIME*, SYSTEMTIME*)>(0x410CA0);
const auto IsKeyFile = reinterpret_cast<BOOL(*)(const char*)>(0x410D10);
const auto GetFileSizeLong = reinterpret_cast<int(*)(const char*)>(0x410D80);
const auto ErrorLog = reinterpret_cast<BOOL(*)(const char*, int)>(0x410DE0);
const auto IsShiftJIS = reinterpret_cast<BOOL(*)(unsigned char)>(0x410E90);

const auto LoadGenericData = reinterpret_cast<BOOL(*)(void)>(0x411390);

// Yes, these are misspelled. You got a problem with that?
const auto PutFramePerSecound = reinterpret_cast<void(*)(void)>(0x412370);
const auto CountFramePerSecound = reinterpret_cast<unsigned long(*)(void)>(0x4123A0);

}
