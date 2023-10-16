#pragma once

#include <vector>
#include <cstdint>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace RequestTypes
{
struct SynchronizedRequest
{
	std::mutex mutex;
	std::condition_variable cv;
	std::atomic_bool fulfilled;
};

struct FlagRequest: SynchronizedRequest
{
	// Input
	std::vector<std::int32_t> flags;
	// Output
	std::vector<unsigned char> result;
};

struct MemoryReadRequest: SynchronizedRequest
{
	// Input
	std::uint32_t address;
	std::uint16_t numBytes;
	// Output
	std::vector<unsigned char> result;
	std::uint32_t errorCode;
};

struct MemoryWriteRequest: SynchronizedRequest
{
	// Input
	std::uint32_t address;
	std::vector<unsigned char> bytes;
	// Output
	std::uint32_t errorCode;
};
} // end namespace RequestTypes
