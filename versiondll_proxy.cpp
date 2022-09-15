#define WIN32_LEAN_AND_MEAN
#include <ShlObj.h>
#include <string>

static HMODULE getRealVersionDLL()
{
    static HMODULE realVersionDLL = NULL;

    if (realVersionDLL == NULL)
    {
        // Get path to system32
        PWSTR system32;
        if (SHGetKnownFolderPath(FOLDERID_SystemX86, 0, NULL, &system32) == S_OK)
        {
            std::wstring versionDLLPath{system32};
            versionDLLPath += L"\\version.dll";

            realVersionDLL = LoadLibraryW(versionDLLPath.c_str());
            if (realVersionDLL)
                OutputDebugStringW((L"CS DLL Mod: Loaded version.dll from " + versionDLLPath).c_str());
            else
                OutputDebugStringW((L"CS DLL Mod: Failed to load version.dll! Path: " + versionDLLPath).c_str());
        }
        else
            OutputDebugStringW(L"CS DLL Mod: Failed to get system directory");
        CoTaskMemFree(system32);
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
