#pragma once

#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <any>

namespace csmulti
{
class RequestQueue
{
public:
	struct Request
	{
		enum class RequestType { SCRIPT, EVENTNUM, FLAGS, MEMREAD, MEMWRITE } type;
		std::any data;
	};
private:
	std::queue<Request> pendingRequests;
	std::mutex requestMutex;

public:
	RequestQueue() = default;
	void push(Request request);
	void fulfillAll();
};
} // end namespace csmulti
