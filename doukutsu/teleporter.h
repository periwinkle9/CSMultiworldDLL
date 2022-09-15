#pragma once

#include "types.h"

namespace csvanilla
{

// Structures
struct PERMITSTAGE
{
	int index;
	int event;
};

// Functions
const auto ClearPermitStage = reinterpret_cast<void(*)(void)>(0x41D610);
const auto AddPermitStage = reinterpret_cast<BOOL(*)(int, int)>(0x41D630);
const auto SubPermitStage = reinterpret_cast<BOOL(*)(int)>(0x41D6A0);
const auto MoveStageSelectCursor = reinterpret_cast<void(*)(void)>(0x41D740);
const auto PutStageSelectObject = reinterpret_cast<void(*)(void)>(0x41D840);
const auto StageSelectLoop = reinterpret_cast<int(*)(int*)>(0x41DA00);

// Global variables
static auto& gPermitStage = *reinterpret_cast<PERMITSTAGE(*)[8]>(0x4A5500);

}