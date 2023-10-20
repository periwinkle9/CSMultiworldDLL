#pragma once

#include <string>
#include <guiddef.h>

namespace csmulti
{
class UUID
{
	IID uuid;
	bool initialized;
public:
	UUID() : uuid{}, initialized{false} {}
	void load();
	bool isInitialized() const { return initialized; }
	const IID& get() const { return uuid; }
	std::string string() const;
};
}
