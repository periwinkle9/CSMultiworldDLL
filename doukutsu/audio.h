#pragma once

#include <dsound.h>

namespace csvanilla
{

// Functions
const auto InitDirectSound = reinterpret_cast<BOOL(*)(HWND)>(0x4200C0);
const auto EndDirectSound = reinterpret_cast<void(*)(void)>(0x4201A0);
const auto InitSoundObject = reinterpret_cast<BOOL(*)(LPCSTR, int)>(0x420240);
const auto LoadSoundObject = reinterpret_cast<BOOL(*)(LPCSTR, int)>(0x420390);
const auto PlaySoundObject = reinterpret_cast<void(*)(int, int)>(0x420640);
const auto ChangeSoundFrequency = reinterpret_cast<void(*)(int, DWORD)>(0x420720);
const auto ChangeSoundVolume = reinterpret_cast<void(*)(int, long)>(0x420760);
const auto ChangeSoundPan = reinterpret_cast<void(*)(int, long)>(0x4207A0);

const auto ChangeMusic = reinterpret_cast<void(*)(int)>(0x420EE0);
const auto ReCallMusic = reinterpret_cast<void(*)(void)>(0x420F50);

// Global variables
static LPDIRECTSOUND& lpDS = *reinterpret_cast<LPDIRECTSOUND*>(0x4A57E8);
static LPDIRECTSOUNDBUFFER& lpPRIMARYBUFFER = *reinterpret_cast<LPDIRECTSOUNDBUFFER*>(0x4A57EC);
static auto& lpSECONDARYBUFFER = *reinterpret_cast<LPDIRECTSOUNDBUFFER(*)[160]>(0x4A5568);

static int& gMusicNo = *reinterpret_cast<int*>(0x4A57F4);
static unsigned int& gOldPos = *reinterpret_cast<unsigned int*>(0x4A57F8);
static int& gOldNo = *reinterpret_cast<int*>(0x4A57FC);
static auto& gMusicTable = *reinterpret_cast<const char* (*)[42]>(0x4981E8);

}