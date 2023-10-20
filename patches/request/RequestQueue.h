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
	std::queue<Request> pendingTSC;
	std::mutex requestMutex;
	std::mutex tscMutex;

public:
	RequestQueue() = default;
	void push(Request request);
	void pushMultiple(const std::vector<Request>& requests);
	bool tryPopTSC(Request& poppedValue);
	void clearTSCQueue();
	void fulfillAll();
};
} // end namespace csmulti
