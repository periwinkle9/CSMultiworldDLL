#pragma once

#include "types.h"

namespace csvanilla
{

// Structures
struct VALUEVIEW
{
	BOOL flag;
	int* px;
	int* py;
	int offset_y;
	int value;
	int count;
	RECT rect;
};

// Functions
const auto ClearValueView = reinterpret_cast<void(*)(void)>(0x425BC0);
const auto SetValueView = reinterpret_cast<void(*)(int*, int*, int)>(0x425BF0);
const auto ActValueView = reinterpret_cast<void(*)(void)>(0x426360);
const auto PutValueView = reinterpret_cast<void(*)(int, int)>(0x426430);

// Global variables
static auto& gVV = *reinterpret_cast<VALUEVIEW(*)[16]>(0x4A5F98);
static int& gVVIndex = *reinterpret_cast<int*>(0x4A5F90);

}