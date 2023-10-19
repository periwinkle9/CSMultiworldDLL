#pragma once

#include <atomic>

void applyGameHooks();
void applyTSCHooks();
void hookGameLoops();
void patch60fps();
enum class GameMode {INIT = -1, OPENING, TITLE, ACTION, INVENTORY, TELEPORTER, MINIMAP, ISLAND_FALLING, ESCAPE};
extern std::atomic<GameMode> currentGameMode;
