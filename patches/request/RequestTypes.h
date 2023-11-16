#pragma once

#include <vector>
#include <cstdint>
#include <utility>
#include <string_view>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace csmulti::RequestTypes
{
struct SynchronizedRequest
{
	std::mutex mutex;
	std::condition_variable cv;
	std::atomic_bool fulfilled;

	SynchronizedRequest() = default;
	virtual ~SynchronizedRequest() = default;
	virtual void fulfill() = 0;
};

struct FlagReadRequest: SynchronizedRequest
{
	// Input
	std::vector<std::int32_t> flags;
	// Output
	std::vector<unsigned char> result;

	void fulfill() override;
};

struct MemoryReadRequest: SynchronizedRequest
{
	// Input
	std::uint32_t address;
	std::uint16_t numBytes;
	// Output
	std::vector<unsigned char> result;
	std::uint32_t errorCode;

	void fulfill() override;
};

struct MemoryWriteRequest: SynchronizedRequest
{
	// Input
	std::uint32_t address;
	std::vector<unsigned char> bytes;
	// Output
	std::uint32_t errorCode;

	void fulfill() override;
};

struct GetCurrentMapRequest: SynchronizedRequest
{
	// Output
	std::string_view mapName;

	void fulfill() override;
};
} // end namespace RequestTypes

namespace csmulti
{
class Request
{
	std::shared_ptr<RequestTypes::SynchronizedRequest> requestData;
public:
	Request() = default;
	Request(RequestTypes::SynchronizedRequest* data) : requestData{data} {}
	template <typename ReqType>
	Request(std::shared_ptr<ReqType> req) : requestData{std::move(req)} {}
	Request(const Request&) = default;
	Request(Request&&) = default;
	~Request() = default;
	Request& operator=(const Request&) = default;
	Request& operator=(Request&&) = default;

	void fulfill() { requestData->fulfill(); }
};
} // end namespace csmulti
