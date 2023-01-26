#include "windows_h_wrapper.h"
#include <shlwapi.h>

static HMODULE getRealVersionDLL()
{
    static HMODULE realVersionDLL = NULL;

    if (realVersionDLL == NULL)
    {
        // Get path to system32
        wchar_t system32[MAX_PATH];
        
        if (GetSystemDirectoryW(system32, MAX_PATH) && PathAppendW(system32, L"version.dll"))
        {
            realVersionDLL = LoadLibraryW(system32);
            if (realVersionDLL)
                OutputDebugStringW(L"CS DLL Mod: Loaded system version.dll");
            else
                OutputDebugStringW(L"CS DLL Mod: Failed to load system version.dll!");
        }
        else
            OutputDebugStringW(L"CS DLL Mod: Failed to get system directory");
    }

    return realVersionDLL;
}

extern "C" {

DWORD APIENTRY Real_GetFileVersionInfoSizeA(LPCSTR lptstrFilename, LPDWORD lpdwHandle)
{
    using Func = decltype(&Real_GetFileVersionInfoSizeA);
    static Func addr = nullptr;

    if (!addr)
        addr = (Func)GetProcAddress(getRealVersionDLL(), "GetFileVersionInfoSizeA");
    return addr(lptstrFilename, lpdwHandle);
}

BOOL APIENTRY Real_GetFileVersionInfoA(LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData)
{
    using Func = decltype(&Real_GetFileVersionInfoA);
    static Func addr = nullptr;

    if (!addr)
        addr = (Func)GetProcAddress(getRealVersionDLL(), "GetFileVersionInfoA");

    return addr(lptstrFilename, dwHandle, dwLen, lpData);
}

BOOL APIENTRY Real_VerQueryValueA(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen)
{
    using Func = decltype(&Real_VerQueryValueA);
    static Func addr = nullptr;

    if (!addr)
        addr = (Func)GetProcAddress(getRealVersionDLL(), "VerQueryValueA");

    return addr(pBlock, lpSubBlock, lplpBuffer, puLen);
}

// Apparently these are needed too???
DWORD APIENTRY Real_GetFileVersionInfoSizeW( LPCWSTR lptstrFilename, LPDWORD lpdwHandle)
{
    using Func = decltype(&Real_GetFileVersionInfoSizeW);
    static Func addr = nullptr;

    if (!addr)
        addr = (Func)GetProcAddress(getRealVersionDLL(), "GetFileVersionInfoSizeW");

    return addr(lptstrFilename, lpdwHandle);
}

BOOL APIENTRY Real_GetFileVersionInfoW(LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData)
{
    using Func = decltype(&Real_GetFileVersionInfoW);
    static Func addr = nullptr;

    if (!addr)
        addr = (Func)GetProcAddress(getRealVersionDLL(), "GetFileVersionInfoW");

    return addr(lptstrFilename, dwHandle, dwLen, lpData);
}

BOOL APIENTRY Real_VerQueryValueW(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen)
{
    using Func = decltype(&Real_VerQueryValueW);
    static Func addr = nullptr;

    if (!addr)
        addr = (Func)GetProcAddress(getRealVersionDLL(), "VerQueryValueW");

    return addr(pBlock, lpSubBlock, lplpBuffer, puLen);
}

} // extern "C"
