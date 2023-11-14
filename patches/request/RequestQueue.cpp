#include "RequestQueue.h"
#include "RequestTypes.h"
#include <utility>
#include <stdexcept>
#include <sstream>
#include <format>
#include <memory>
#include "../Multiworld.h"
#include "doukutsu/flags.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace csmulti
{
void RequestQueue::push(Request request)
{
	using RT = Request::RequestType;
	switch (request.type)
	{
	case RT::SCRIPT:
	case RT::EVENTNUM:
	{
		std::scoped_lock lock{tscMutex};
		pendingTSC.push(std::move(request));
		break;
	}
	case RT::FLAGS:
	case RT::MEMREAD:
	case RT::MEMWRITE:
	{
		std::scoped_lock lock{requestMutex};
		pendingRequests.push(std::move(request));
		break;
	}
	default:
		throw std::logic_error("Unrecognized request type");
	}
}

void RequestQueue::pushMultiple(const std::vector<Request>& requests)
{
	std::scoped_lock lock{requestMutex, tscMutex};
	for (const Request& request : requests)
	{
		using RT = Request::RequestType;
		switch (request.type)
		{
		case RT::SCRIPT:
		case RT::EVENTNUM:
			pendingTSC.push(request);
			break;
		case RT::FLAGS:
		case RT::MEMREAD:
		case RT::MEMWRITE:
			pendingRequests.push(request);
			break;
		default:
			throw std::logic_error("Unrecognized request type");
		}
	}
}

bool RequestQueue::tryPopTSC(Request& poppedRequest)
{
	std::unique_lock<std::mutex> lock{tscMutex, std::try_to_lock};
	if (lock)
	{
		if (!pendingTSC.empty())
		{
			poppedRequest = std::move(pendingTSC.front());
			pendingTSC.pop();
			return true;
		}
	}
	return false;
}

void RequestQueue::clearTSCQueue()
{
	logger().logDebug("Clearing TSC event queue");
	std::unique_lock<std::mutex> lock{tscMutex};
	// Heh, std::queue doesn't have a .clear() member function, so let's just swap it out for an empty container
	using std::swap;
	decltype(pendingTSC) empty;
	swap(pendingTSC, empty);
}

static void fulfill(RequestQueue::Request& request)
{
	using RT = RequestQueue::Request::RequestType;
	using namespace RequestTypes;
	switch (request.type)
	{
	case RT::FLAGS:
	{
		std::shared_ptr<FlagRequest> req = std::any_cast<std::shared_ptr<FlagRequest>>(request.data);
		if (req == nullptr)
			throw std::logic_error("Null request data");

		{
			std::ostringstream oss;
			oss << "Acquiring lock to fulfill request for flags: ";
			for (auto flag : req->flags)
				oss << flag << ' ';
			logger().logTrace(oss.str());
		}
		{
			std::scoped_lock<std::mutex> lock{req->mutex};
			auto getFlag = [](std::int32_t flagNum) -> bool { return (csvanilla::gFlagNPC[flagNum / 8] & (1 << (flagNum % 8))) != 0; };
			req->result.reserve(req->flags.size());
			for (std::int32_t flag : req->flags)
				req->result.push_back(getFlag(flag));
			req->fulfilled = true;
		}
		req->cv.notify_one();
		logger().logTrace("Flag request fulfilled, server thread notified");
		break;
	}
	case RT::MEMREAD:
	{
		std::shared_ptr<MemoryReadRequest> req = std::any_cast<std::shared_ptr<MemoryReadRequest>>(request.data);
		if (req == nullptr)
			throw std::logic_error("Null request data");

		logger().logTrace(std::format("Acquiring lock to fulfill mem request at {:#x}, {} bytes", req->address, req->numBytes));
		{
			std::scoped_lock<std::mutex> lock{req->mutex};
			req->result.resize(req->numBytes);
			if (ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<const void*>(req->address), req->result.data(), req->numBytes, nullptr))
				req->errorCode = 0;
			else
				req->errorCode = GetLastError();
			req->fulfilled = true;
		}
		req->cv.notify_one();
		logger().logTrace("Memory read request fulfilled, server notified");
		break;
	}
	case RT::MEMWRITE:
	{
		std::shared_ptr<MemoryWriteRequest> req = std::any_cast<std::shared_ptr<MemoryWriteRequest>>(request.data);
		if (req == nullptr)
			throw std::logic_error("Null request data");

		logger().logTrace(std::format("Acquiring lock to fulfilling mem request at {:#x}, {} bytes", req->address, req->bytes.size()));
		{
			std::scoped_lock<std::mutex> lock{req->mutex};
			if (WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(req->address), req->bytes.data(), req->bytes.size(), nullptr))
				req->errorCode = 0;
			else
				req->errorCode = GetLastError();
			req->fulfilled = true;
		}
		req->cv.notify_one();
		logger().logTrace("Memory write request fulfilled, server notified");
		break;
	}
	default:
		throw std::logic_error("Wrong request handler");
	}
}

void RequestQueue::fulfillAll()
{
	std::scoped_lock lock{requestMutex};
	while (!pendingRequests.empty())
	{
		Request request = std::move(pendingRequests.front());
		pendingRequests.pop();
		fulfill(request);
	}
}
} // end namespace csmulti
