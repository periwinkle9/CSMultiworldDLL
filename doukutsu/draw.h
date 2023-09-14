#pragma once

#include <Windows.h>
#include <ddraw.h>

namespace csvanilla
{

// Structures
struct SurfaceMetadata // Not the original struct name
{
	char name[20];
	unsigned int width;
	unsigned int height;
	int type;
	BOOL bSystem;
};

// Functions
const auto SetClientOffset = reinterpret_cast<void(*)(int, int)>(0x40B320);
const auto Flip_SystemTask = reinterpret_cast<BOOL(*)(HWND)>(0x40B340);
const auto StartDirectDraw = reinterpret_cast<BOOL(*)(HWND, int, int)>(0x40B450);
const auto EndDirectDraw = reinterpret_cast<void(*)(HWND)>(0x40B6C0);
const auto ReleaseSurface = reinterpret_cast<void(*)(int)>(0x40B7A0);
const auto MakeSurface_Resource = reinterpret_cast<BOOL(*)(const char*, int)>(0x40B800);
const auto MakeSurface_File = reinterpret_cast<BOOL(*)(const char*, int)>(0x40BAC0);
const auto ReloadBitmap_Resource = reinterpret_cast<BOOL(*)(const char*, int)>(0x40BE10);
const auto ReloadBitmap_File = reinterpret_cast<BOOL(*)(const char*, int)>(0x40BFD0);
const auto MakeSurface_Generic = reinterpret_cast<BOOL(*)(int, int, int, BOOL)>(0x40C1D0);
const auto BackupSurface = reinterpret_cast<void(*)(int, const ::RECT*)>(0x40C320);
const auto PutBitmap3 = reinterpret_cast<void(*)(const ::RECT*, int, int, const ::RECT*, int)>(0x40C3C0);
const auto PutBitmap4 = reinterpret_cast<void(*)(const ::RECT*, int, int, const ::RECT*, int)>(0x40C5B0);
const auto Surface2Surface = reinterpret_cast<void(*)(int, int, const ::RECT*, int, int)>(0x40C7A0);
const auto GetCortBoxColor = reinterpret_cast<unsigned long(*)(COLORREF)>(0x40C8B0);
const auto CortBox = reinterpret_cast<void(*)(const ::RECT*, unsigned long)>(0x40C9E0);
const auto CortBox2 = reinterpret_cast<void(*)(const ::RECT*, unsigned long, int)>(0x40CA80);
//const auto out = reinterpret_cast<BOOL(*)(char)>(0x40CB30); // Probably nobody needs this
const auto RestoreSurfaces = reinterpret_cast<int(*)(void)>(0x40CB60);
const auto InitTextObject = reinterpret_cast<void(*)(const char*)>(0x40CD50);
const auto PutText = reinterpret_cast<void(*)(int, int, const char*, unsigned long)>(0x40CE00);
const auto PutText2 = reinterpret_cast<void(*)(int, int, const char*, unsigned long, int)>(0x40CEB0);
const auto EndTextObject = reinterpret_cast<void(*)(void)>(0x40CF70);
const auto IsEnableBitmap = reinterpret_cast<BOOL(*)(const char*)>(0x4112E0);

// Global variables
static ::RECT& grcGame = *reinterpret_cast<::RECT*>(0x48F91C);
static ::RECT& grcFull = *reinterpret_cast<::RECT*>(0x48F92C);
// Not original names below
static int& window_magnification = *reinterpret_cast<int*>(0x48F914); // Window magnification
static BOOL& is_fullscreen = *reinterpret_cast<BOOL*>(0x49CDC0);
static LPDIRECTDRAW& lpDD = *reinterpret_cast<LPDIRECTDRAW*>(0x49D37C);
static LPDIRECTDRAWSURFACE& frontbuffer = *reinterpret_cast<LPDIRECTDRAWSURFACE*>(0x49D380);
static LPDIRECTDRAWSURFACE& backbuffer = *reinterpret_cast<LPDIRECTDRAWSURFACE*>(0x49D384);
static LPDIRECTDRAWCLIPPER& clipper = *reinterpret_cast<LPDIRECTDRAWCLIPPER*>(0x49CDC4);
static auto& surf = *reinterpret_cast<LPDIRECTDRAWSURFACE(*)[40]>(0x49D388);
static auto& surface_metadata = *reinterpret_cast<SurfaceMetadata(*)[40]>(0x49CDC8);
static ::RECT& backbuffer_rect = *reinterpret_cast<::RECT*>(0x49CDB0);
static int& scaled_window_width = *reinterpret_cast<int*>(0x49D374);
static int& scaled_window_height = *reinterpret_cast<int*>(0x49D378);
static HFONT& font = *reinterpret_cast<HFONT*>(0x49D368);
static int& client_x = *reinterpret_cast<int*>(0x49CDAC);
static int& client_y = *reinterpret_cast<int*>(0x49CDA8);
static const char*& extra_text = *reinterpret_cast<const char**>(0x48F93C); // Pointer to (C)Pixel copyright string

}