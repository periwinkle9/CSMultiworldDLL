#pragma once

#include <Windows.h>
// Newer versions of Visual Studio don't support anything earlier than DirectInput8
#if defined(_MSC_VER) && _MSC_VER >= 1500
#define DIRECTINPUT_VERSION 0x800
#else
#define DIRECTINPUT_VERSION 0x500
#endif
#include <dinput.h>

namespace csvanilla
{

// Structures
struct DIRECTINPUTSTATUS
{
	BOOL bLeft;
	BOOL bRight;
	BOOL bUp;
	BOOL bDown;
	BOOL bButton[32];
};
/*
struct DirectInputPair
{
	LPDIRECTINPUTA lpDI;
	LPDIRECTINPUTDEVICE2A device;
};
*/

// Functions
const auto CenteringWindowByParent = reinterpret_cast<BOOL(*)(void*)>(0x410EE0);
const auto LoadWindowRect = reinterpret_cast<BOOL(*)(void*, const char*, BOOL)>(0x410FE0);
const auto SaveWindowRect = reinterpret_cast<BOOL(*)(void*, const char*)>(0x4111F0);

const auto ReleaseDirectInput = reinterpret_cast<void(*)(void)>(0x411E10);
const auto ActivateDirectInput = reinterpret_cast<BOOL(*)(BOOL)>(0x411E60);
const auto InitDirectInput = reinterpret_cast<BOOL(*)(HINSTANCE, HWND)>(0x411EB0);

// Not the original names
const auto FindAndOpenDirectInputDevice = reinterpret_cast<BOOL(*)(HWND)>(0x411EF0);
const auto EnumDevices_Callback = reinterpret_cast<BOOL(CALLBACK*)(LPCDIDEVICEINSTANCE, LPVOID)>(0x411FC0);

const auto GetJoystickStatus = reinterpret_cast<BOOL(*)(DIRECTINPUTSTATUS*)>(0x4120F0);
const auto ResetJoystickStatus = reinterpret_cast<BOOL(*)(void)>(0x412250);
const auto JoystickProc = reinterpret_cast<void(*)(void)>(0x4135E0);

const auto SetWindowName = reinterpret_cast<void(*)(HWND)>(0x412320); // Not the original name
const auto WinMain = reinterpret_cast<int(WINAPI*)(HINSTANCE, HINSTANCE, LPSTR, int)>(0x412420);
const auto InactiveWindow = reinterpret_cast<void(*)(void)>(0x412BC0);
const auto ActiveWindow = reinterpret_cast<void(*)(void)>(0x412BF0);
const auto DragAndDropHandler = reinterpret_cast<BOOL(*)(HWND, WPARAM)>(0x412C30);
const auto WindowProcedure = reinterpret_cast<LRESULT(CALLBACK*)(HWND, UINT, WPARAM, LPARAM)>(0x412CA0);
const auto SystemTask = reinterpret_cast<BOOL(*)(void)>(0x413570);

// Global variables
// Not the original names
static LPDIRECTINPUTA& lpDI = *reinterpret_cast<LPDIRECTINPUTA*>(0x49E1F0);
static LPDIRECTINPUTDEVICE2A& joystick = *reinterpret_cast<LPDIRECTINPUTDEVICE2A*>(0x49E1F4);
static int& joystick_neutral_x = *reinterpret_cast<int*>(0x49E1F8);
static int& joystick_neutral_y = *reinterpret_cast<int*>(0x49E1FC);

static BOOL& gbUseJoystick = *reinterpret_cast<BOOL*>(0x49E45C);

static auto& gModulePath = *reinterpret_cast<char(*)[MAX_PATH]>(0x49E328);
static auto& gDataPath = *reinterpret_cast<char(*)[MAX_PATH]>(0x49E220);

static HWND& ghWnd = *reinterpret_cast<HWND*>(0x49E458);
static BOOL& bActive = *reinterpret_cast<BOOL*>(0x49E468);
static BOOL& bFPS = *reinterpret_cast<BOOL*>(0x49E464);
static BOOL& bFullscreen = *reinterpret_cast<BOOL*>(0x49E460);
static HANDLE& hMutex = *reinterpret_cast<HANDLE*>(0x49E478);
static HINSTANCE ghInstance = *reinterpret_cast<HINSTANCE*>(0x49E44C);
static int& windowWidth = *reinterpret_cast<int*>(0x49E450);
static int& windowHeight = *reinterpret_cast<int*>(0x49E454);
static const char*& lpWindowName = *reinterpret_cast<const char**>(0x493640);

}