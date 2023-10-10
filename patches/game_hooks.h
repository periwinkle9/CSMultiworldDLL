#pragma once

#include <atomic>

void applyTSCHooks();
void hookGameLoops();
enum class GameMode {INIT = -1, OPENING, TITLE, ACTION, INVENTORY, TELEPORTER, MINIMAP, ISLAND_FALLING, ESCAPE};
extern std::atomic<GameMode> currentGameMode;
