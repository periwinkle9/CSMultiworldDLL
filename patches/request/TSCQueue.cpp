#include "TSCQueue.h"
#include <utility>
#include "../Multiworld.h"

namespace csmulti
{
void TSCQueue::push(std::string script)
{
	std::scoped_lock lock{mutex};
	eventQueue.emplace(std::move(script));
}

void TSCQueue::push(const std::vector<std::int32_t>& eventNums)
{
	std::scoped_lock lock{mutex};
	for (int eventNum : eventNums)
		eventQueue.emplace(eventNum);
}

bool TSCQueue::tryPop(Event& poppedValue)
{
	std::unique_lock<std::mutex> lock{mutex, std::try_to_lock};
	if (lock && !eventQueue.empty())
	{
		poppedValue = std::move(eventQueue.front());
		eventQueue.pop();
		return true;
	}
	return false;
}

void TSCQueue::clear()
{
	logger().logDebug("Clearing TSC event queue");
	std::scoped_lock lock{mutex};
	// Heh, std::queue doesn't have a .clear() member function, so let's just swap it out for an empty container
	using std::swap;
	decltype(eventQueue) empty;
	swap(eventQueue, empty);
}
}