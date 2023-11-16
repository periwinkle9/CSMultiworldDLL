#include "RequestQueue.h"
#include "RequestTypes.h"
#include <utility>

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
} // end namespace csmulti
