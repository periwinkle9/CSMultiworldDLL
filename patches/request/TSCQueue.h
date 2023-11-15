#pragma once

#include <queue>
#include <vector>
#include <string>
#include <cstdint>
#include <variant>
#include <mutex>

namespace csmulti
{
class TSCQueue
{
public:
	using Event = std::variant<std::string, std::int32_t>;
private:
	std::queue<Event> eventQueue;
	std::mutex mutex;
public:
	TSCQueue() = default;
	void push(std::string script);
	void push(const std::vector<std::int32_t>& eventNums);
	bool tryPop(Event& poppedValue);
	void clear();
};
} // end namespace csmulti
