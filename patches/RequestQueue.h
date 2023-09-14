#pragma once

#include <string>
#include <queue>
#include <mutex>

struct Request
{
	enum class RequestType { SCRIPT, EVENTNUM } type;
	std::string script;
	int eventNum;
};

class RequestQueue
{
	std::queue<Request> pendingRequests;
	std::mutex mutex;

public:
	RequestQueue() = default;
	void push(Request request);
	bool tryPop(Request& poppedValue);
};

extern RequestQueue* requestQueue;

void initRequestQueue();
void endRequestQueue();
