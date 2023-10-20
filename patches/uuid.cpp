#include "uuid.h"
#include <fstream>
#include <filesystem>
#include <cstring>
#include <string>
#include "doukutsu/window.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <objbase.h>

namespace csmulti
{
IID uuid;
bool uuidInitialized = false;
}
using namespace csmulti;

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
