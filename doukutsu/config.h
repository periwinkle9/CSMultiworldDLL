#pragma once

#include "types.h"

namespace csvanilla
{

// Structures
struct CONFIGDATA
{
	char proof[0x20];
	char font_name[0x40];
	int move_button_mode;
	int attack_button_mode;
	int ok_button_mode;
	int display_mode;
	BOOL bJoystick;
	int joystick_button[8];
};

// Functions
const auto LoadConfigData = reinterpret_cast<BOOL(*)(CONFIGDATA*)>(0x40AD60);
const auto DefaultConfigData = reinterpret_cast<void(*)(CONFIGDATA*)>(0x40AE30);

// Global variables
static const char*& gProof = *reinterpret_cast<const char**>(0x48F908);
static const char*& gConfigName = *reinterpret_cast<const char**>(0x48F90C);

}