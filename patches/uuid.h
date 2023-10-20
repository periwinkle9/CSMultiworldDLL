#pragma once

#include <string>
#include <guiddef.h>

namespace csmulti
{
extern IID uuid;
extern bool uuidInitialized;
}

void loadUUID();
std::string getUUIDString();
