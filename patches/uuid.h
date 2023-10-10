#pragma once

#include <string>
#include <guiddef.h>

extern GUID uuid;
extern bool uuidInitialized;

void loadUUID();
std::string getUUIDString();
void patchUUIDChecks();
