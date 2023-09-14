// dllmain.cpp : Defines the entry point for the DLL application.
// (You shouldn't need to modify anything in this file.)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void applyPatches();

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    try
    {
        if (ul_reason_for_call == DLL_PROCESS_ATTACH)
            applyPatches();
    }
    catch (...)
    {
        return FALSE;
    }

    return TRUE;
}
