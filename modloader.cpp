/* Define MODLOADER_DLL to allow the DLL to be imported by Clownacy's mod loader
   as an alternative to using it standalone as a dinput.dll replacement.
   Note, #define-ing this is not necessary for using both this (as a standalone DLL)
   and the mod loader side-by-side, as the two do not inherently conflict.
*/
//#define MODLOADER_DLL

#ifdef MODLOADER_DLL

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// The mod loader expects to find a 'ModEntry' function to call when loading the DLL.
// For our purposes, though, there is nothing that needs to be implemented here, as
// all of the work is done from DllMain().
// That said, if you want to make use of the mod loader Settings API, you can do that here
// (by copy-pasting the relevant parts of src/common/mod_loader.c, etc.)
extern "C" __declspec(dllexport) void ModEntry(const HMODULE, const struct Settings*, const char* const)
{}

#endif
