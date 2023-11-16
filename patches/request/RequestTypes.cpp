#include "RequestTypes.h"
#include <utility>
#include <sstream>
#include <format>
#include "../Multiworld.h"
#include "doukutsu/flags.h"
#include "doukutsu/map.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace csmulti::RequestTypes
{
void FlagReadRequest::fulfill()
{
	{
		std::ostringstream oss;
		oss << "Acquiring lock to fulfill request for flags: ";
		for (auto flag : flags)
			oss << flag << ' ';
		logger().logTrace(oss.str());
	}
	{
		std::scoped_lock lock{mutex};
		auto getFlag = [](std::int32_t flagNum) -> bool { return (csvanilla::gFlagNPC[flagNum / 8] & (1 << (flagNum % 8))) != 0; };
		result.reserve(flags.size());
		for (std::int32_t flag : flags)
			result.push_back(getFlag(flag));
		fulfilled = true;
	}
	cv.notify_one();
	logger().logTrace("Flag request fulfilled, server thread notified");
}

void MemoryReadRequest::fulfill()
{
	logger().logTrace(std::format("Acquiring lock to fulfill mem request at {:#x}, {} bytes", address, numBytes));
	{
		std::scoped_lock lock{mutex};
		result.resize(numBytes);
		if (ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<const void*>(address), result.data(), numBytes, nullptr))
			errorCode = 0;
		else
			errorCode = GetLastError();
		fulfilled = true;
	}
	cv.notify_one();
	logger().logTrace("Memory read request fulfilled, server notified");
}

void MemoryWriteRequest::fulfill()
{
	logger().logTrace(std::format("Acquiring lock to fulfilling mem request at {:#x}, {} bytes", address, bytes.size()));
	{
		std::scoped_lock lock{mutex};
		if (WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(address), bytes.data(), bytes.size(), nullptr))
			errorCode = 0;
		else
			errorCode = GetLastError();
		fulfilled = true;
	}
	cv.notify_one();
	logger().logTrace("Memory write request fulfilled, server notified");
}

void GetCurrentMapRequest::fulfill()
{
	logger().logTrace("Acquiring lock to fulfill current map name request");
	{
		std::scoped_lock lock{mutex};
		mapName = std::string_view{csvanilla::gTMT[csvanilla::gStageNo].map};
		fulfilled = true;
	}
	cv.notify_one();
	logger().logTrace("Map name request fulfilled, server notified");
}
} // end namespace csmulti::RequestTypes