#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Shlwapi.h>

static HMODULE getRealDInputDLL()
{
    static HMODULE realDInputDLL = NULL;

    if (realDInputDLL == NULL)
    {
        // Get path to system32
        wchar_t system32[MAX_PATH];

        if (GetSystemDirectoryW(system32, MAX_PATH) && PathAppendW(system32, L"dinput.dll"))
        {
            realDInputDLL = LoadLibraryW(system32);
            if (realDInputDLL)
                OutputDebugStringW(L"CS DLL Mod: Loaded system dinput.dll");
            else
                OutputDebugStringW(L"CS DLL Mod: Failed to load system dinput.dll!");
        }
        else
            OutputDebugStringW(L"CS DLL Mod: Failed to get system directory");
    }

    return realDInputDLL;
}

typedef struct IDirectInputA* LPDIRECTINPUTA;

extern "C" {

HRESULT WINAPI Real_DirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA* ppDI, LPUNKNOWN punkOuter)
{
    using Func = decltype(&Real_DirectInputCreateA);
    static Func addr = nullptr;

    if (!addr)
        addr = (Func)GetProcAddress(getRealDInputDLL(), "DirectInputCreateA");

    return addr(hinst, dwVersion, ppDI, punkOuter);
}
HRESULT WINAPI Real_DirectInputCreateW(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA* ppDI, LPUNKNOWN punkOuter)
{
    using Func = decltype(&Real_DirectInputCreateW);
    static Func addr = nullptr;

    if (!addr)
        addr = (Func)GetProcAddress(getRealDInputDLL(), "DirectInputCreateW");

    return addr(hinst, dwVersion, ppDI, punkOuter);
}

HRESULT WINAPI Real_DirectInputCreateEx(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
{
    using Func = decltype(&Real_DirectInputCreateEx);
    static Func addr = nullptr;

    if (!addr)
        addr = (Func)GetProcAddress(getRealDInputDLL(), "DirectInputCreateEx");

    return addr(hinst, dwVersion, riidltf, ppvOut, punkOuter);
}

HRESULT WINAPI Real_DllCanUnloadNow(void)
{
    using Func = decltype(&Real_DllCanUnloadNow);
    static Func addr = nullptr;

    if (!addr)
        addr = (Func)GetProcAddress(getRealDInputDLL(), "DllCanUnloadNow");

    return addr();
}

HRESULT WINAPI Real_DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    using Func = decltype(&Real_DllGetClassObject);
    static Func addr = nullptr;

    if (!addr)
        addr = (Func)GetProcAddress(getRealDInputDLL(), "DllGetClassObject");

    return addr(rclsid, riid, ppv);
}

HRESULT WINAPI Real_DllRegisterServer(void)
{
    using Func = decltype(&Real_DllRegisterServer);
    static Func addr = nullptr;

    if (!addr)
        addr = (Func)GetProcAddress(getRealDInputDLL(), "DllRegisterServer");

    return addr();
}

HRESULT WINAPI Real_DllUnregisterServer(void)
{
    using Func = decltype(&Real_DllUnregisterServer);
    static Func addr = nullptr;

    if (!addr)
        addr = (Func)GetProcAddress(getRealDInputDLL(), "DllUnregisterServer");

    return addr();
}

} // extern "C"
