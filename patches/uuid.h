#pragma once

#include <string>
#include <guiddef.h>

extern IID uuid;
extern bool uuidInitialized;

void loadUUID();
std::string getUUIDString();
