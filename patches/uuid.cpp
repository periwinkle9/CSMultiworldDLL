#include "uuid.h"
#include <fstream>
#include <filesystem>
#include <cstring>
#include <string>
#include "doukutsu/map.h"
#include "doukutsu/profile.h"
#include "doukutsu/window.h"
#include "patch_utils.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <objbase.h>

IID uuid;
bool uuidInitialized = false;

void loadUUID()
{
	if (!uuidInitialized)
	{
		std::filesystem::path uuidPath{csvanilla::gDataPath};
		uuidPath /= "uuid.txt";
		std::wifstream ifs{uuidPath};
		std::wstring line;
		if (ifs && std::getline(ifs, line) && IIDFromString(line.c_str(), &uuid) == S_OK)
		{
			uuidInitialized = true;
		}
	}
}

std::string getUUIDString()
{
	if (uuidInitialized)
	{
		// Having to use wchar_t here makes it slightly annoying, but whatever
		wchar_t buf[50] = {};
		int numChars = StringFromGUID2(uuid, buf, 50);
		char convertedBuf[100] = {};
		if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, buf, numChars, convertedBuf, sizeof convertedBuf, nullptr, nullptr))
			return std::string(convertedBuf);
	}
	return std::string{};
}

namespace
{

void* ProfileUUID = &csvanilla::gMapping[sizeof csvanilla::gMapping - sizeof uuid]; // Use the last 16 bytes of map flag space

// Replacement for vanilla StartMapping()
void StartMapping()
{
	using csvanilla::gMapping;
	std::memset(gMapping, 0, sizeof gMapping);
	if (uuidInitialized)
		std::memcpy(ProfileUUID, &uuid, sizeof uuid);
}

BOOL LoadProfile(const char* name)
{
	if (!csvanilla::LoadProfile(name))
		return FALSE;
	if (uuidInitialized && std::memcmp(ProfileUUID, &uuid, sizeof uuid) != 0)
	{
		MessageBoxA(csvanilla::ghWnd, "The save file you are loading appears to have been created from a different world.\n"
			"A new game will be started instead.", "UUID Mismatch", MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	}
	return TRUE;
}

}

void patchUUIDChecks()
{
	patcher::replaceFunction(csvanilla::StartMapping, StartMapping);
	const patcher::dword LoadProfileCalls[] = {0x410487, 0x412C7A, 0x424E08};
	for (auto addr : LoadProfileCalls)
		patcher::writeCall(addr, LoadProfile);
}
