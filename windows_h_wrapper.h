#pragma once

// In Visual Studio 2019, when targeting for Windows XP, the typedef below is needed to avoid compilation errors

//typedef struct IUnknown IUnknown;
#ifndef ALL_WIN_H
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
