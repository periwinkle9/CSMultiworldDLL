#pragma once

#include <queue>
#include <mutex>
#include "RequestTypes.h"

namespace csmulti
{
class RequestQueue
{
private:
	std::queue<Request> pendingRequests;
	std::mutex mutex;

public:
	RequestQueue() = default;
	void push(Request request);
	void fulfillAll();
};
} // end namespace csmulti
