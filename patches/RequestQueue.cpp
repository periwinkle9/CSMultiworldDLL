#include "RequestQueue.h"
#include "RequestTypes.h"
#include <utility>
#include <stdexcept>
#include <sstream>
#include <format>
#include "Logger.h"
#include "doukutsu/flags.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

RequestQueue* requestQueue;

void initRequestQueue()
{
	requestQueue = new RequestQueue;
}
void endRequestQueue()
{
	// This is called after everything else shuts down, so it should be safe
	delete requestQueue;
	requestQueue = nullptr;
}

void RequestQueue::push(Request request)
{
	std::unique_lock<std::mutex> lock{requestMutex};
	pendingRequests.push(std::move(request));
}

void RequestQueue::pushMultiple(const std::vector<Request>& requests)
{
	std::unique_lock<std::mutex> lock{requestMutex};
	for (const Request& request : requests)
		pendingRequests.push(request);
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
		FlagRequest* req = std::any_cast<FlagRequest*>(request.data);
		if (req == nullptr)
			throw std::logic_error("Null request data");

		{
			std::ostringstream oss;
			oss << "Acquiring lock to fulfill request for flags: ";
			for (auto flag : req->flags)
				oss << flag << ' ';
			logger.logDebug(oss.str());
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
		logger.logDebug("Flag request fulfilled, server thread notified");
		break;
	}
	case RT::MEMREAD:
	{
		MemoryReadRequest* req = std::any_cast<MemoryReadRequest*>(request.data);
		if (req == nullptr)
			throw std::logic_error("Null request data");

		logger.logDebug(std::format("Acquiring lock to fulfill mem request at {:#x}, {} bytes", req->address, req->numBytes));
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
		logger.logDebug("Memory read request fulfilled, server notified");
		break;
	}
	case RT::MEMWRITE:
	{
		MemoryWriteRequest* req = std::any_cast<MemoryWriteRequest*>(request.data);
		if (req == nullptr)
			throw std::logic_error("Null request data");

		logger.logDebug(std::format("Acquiring lock to fulfilling mem request at {:#x}, {} bytes", req->address, req->bytes.size()));
		{
			std::scoped_lock<std::mutex> lock{req->mutex};
			if (WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(req->address), req->bytes.data(), req->bytes.size(), nullptr))
				req->errorCode = 0;
			else
				req->errorCode = GetLastError();
			req->fulfilled = true;
		}
		req->cv.notify_one();
		logger.logDebug("Memory write request fulfilled, server notified");
		break;
	}
	default:
		throw std::logic_error("Wrong request handler");
	}
}

void RequestQueue::fulfillAll()
{
	std::vector<Request> newTSCRequests;
	{
		std::scoped_lock<std::mutex> lock{requestMutex};
		while (!pendingRequests.empty())
		{
			Request request = std::move(pendingRequests.front());
			pendingRequests.pop();
			if (request.type == Request::RequestType::SCRIPT || request.type == Request::RequestType::EVENTNUM)
				newTSCRequests.push_back(std::move(request));
			else
				fulfill(request);
		}
	}
	if (!newTSCRequests.empty())
	{
		logger.logDebug(std::format("Request handler: Queued {} TSC events for execution", newTSCRequests.size()));
		std::scoped_lock<std::mutex> lock{tscMutex};
		for (Request& req : newTSCRequests)
			pendingTSC.push(std::move(req));
	}
}
