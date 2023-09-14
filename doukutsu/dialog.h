#pragma once

#include <Windows.h>

namespace csvanilla
{

// Functions
const auto VersionDialog = reinterpret_cast<DLGPROC>(0x40AEC0);
const auto DebugMuteDialog = reinterpret_cast<DLGPROC>(0x40AFC0);
const auto DebugSaveDialog = reinterpret_cast<DLGPROC>(0x40B1D0);
const auto QuitDialog = reinterpret_cast<DLGPROC>(0x40B290);

// Global variables
static const char*& gVersionString = *reinterpret_cast<const char**>(0x48F910);

}