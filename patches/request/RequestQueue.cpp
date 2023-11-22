#include "RequestQueue.h"
#include "RequestTypes.h"
#include <utility>
#include "../Multiworld.h"

namespace csmulti
{
void RequestQueue::push(Request request)
{
	std::scoped_lock lock{mutex};
	pendingRequests.push(std::move(request));
}

void RequestQueue::fulfillAll()
{
	std::scoped_lock lock{mutex};
	while (!pendingRequests.empty())
	{
		Request request = std::move(pendingRequests.front());
		pendingRequests.pop();
		request.fulfill();
	}
}

void RequestQueue::cancelAll()
{
	std::scoped_lock lock{mutex};
	if (!pendingRequests.empty())
		logger().logInfo("Canceling all outstanding server requests");
	while (!pendingRequests.empty())
	{
		pendingRequests.front().cancel();
		pendingRequests.pop();
	}
}
} // end namespace csmulti
