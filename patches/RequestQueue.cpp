#include "RequestQueue.h"
#include <utility>

RequestQueue* requestQueue;

void initRequestQueue()
{
	requestQueue = new RequestQueue;
}
void endRequestQueue()
{
	// TODO how to delete requestQueue in a thread-safe way?
	// And what to do about unfulfilled requests?
	delete requestQueue;
}

void RequestQueue::push(Request request)
{
	std::unique_lock<std::mutex> lock{mutex};
	pendingRequests.push(std::move(request));
}

bool RequestQueue::tryPop(Request& poppedRequest)
{
	std::unique_lock<std::mutex> lock{mutex, std::try_to_lock};
	if (lock)
	{
		if (!pendingRequests.empty())
		{
			poppedRequest = std::move(pendingRequests.front());
			pendingRequests.pop();
			return true;
		}
	}
	return false;
}
