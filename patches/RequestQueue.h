#pragma once

#include <string>
#include <vector>
#include <queue>
#include <mutex>

class RequestQueue
{
public:
	struct Request
	{
		enum class RequestType { SCRIPT, EVENTNUM } type;
		std::string script;
		int eventNum;
	};
private:
	std::queue<Request> pendingRequests;
	std::mutex mutex;

public:
	RequestQueue() = default;
	void push(Request request);
	void pushMultiple(const std::vector<Request>& requests);
	bool tryPop(Request& poppedValue);
};

extern RequestQueue* requestQueue;

void initRequestQueue();
void endRequestQueue();
