#pragma once

#include "types.h"

namespace csvanilla
{

// Structures
struct MAPDATA
{
	unsigned char* data;
	unsigned char atrb[0x101];
	short width;
	short length;
};
struct MapName
{
	BOOL flag;
	int wait;
	char name[0x20];
};
struct TRANSMAPTABLE
{
	char parts[0x20];
	char map[0x20];
	int bkType;
	char back[0x20];
	char npc[0x20];
	char boss[0x20];
	signed char boss_no;
	char name[0x20];
};

// Functions
const auto InitMapData2 = reinterpret_cast<BOOL(*)(void)>(0x413750);
const auto LoadMapData2 = reinterpret_cast<BOOL(*)(const char*)>(0x413770);
const auto LoadAttributeData = reinterpret_cast<BOOL(*)(const char*)>(0x4138A0);
const auto EndMapData = reinterpret_cast<void(*)(void)>(0x413930);
const auto ReleasePartsImage = reinterpret_cast<void(*)(void)>(0x413950);
const auto GetMapData = reinterpret_cast<void(*)(unsigned char**, short*, short*)>(0x413960);
const auto GetAttribute = reinterpret_cast<unsigned char(*)(int, int)>(0x4139A0);
const auto DeleteMapParts = reinterpret_cast<void(*)(int, int)>(0x413A00);
const auto ShiftMapParts = reinterpret_cast<void(*)(int, int)>(0x413A20);
const auto ChangeMapParts = reinterpret_cast<BOOL(*)(int, int, unsigned char)>(0x413A60);
const auto PutStage_Back = reinterpret_cast<void(*)(int, int)>(0x413AF0);
const auto PutStage_Front = reinterpret_cast<void(*)(int, int)>(0x413C60);
const auto PutMapDataVector = reinterpret_cast<void(*)(int, int)>(0x413E40);

const auto ReadyMapName = reinterpret_cast<void(*)(const char*)>(0x4140F0);
const auto PutMapName = reinterpret_cast<void(*)(BOOL)>(0x414250);
const auto StartMapName = reinterpret_cast<void(*)(void)>(0x414310);
const auto RestoreMapName = reinterpret_cast<void(*)(void)>(0x414330);

const auto WriteMiniMapLine = reinterpret_cast<void(*)(int)>(0x4143C0);
const auto MiniMapLoop = reinterpret_cast<int(*)(void)>(0x414640);
const auto IsMapping = reinterpret_cast<BOOL(*)(void)>(0x414B00);
const auto StartMapping = reinterpret_cast<void(*)(void)>(0x414B20);
const auto SetMapping = reinterpret_cast<void(*)(int)>(0x414B40);

const auto TransferStage = reinterpret_cast<BOOL(*)(int, int, int, int)>(0x420BE0);

// Global variables
static MAPDATA& gMap = *reinterpret_cast<MAPDATA*>(0x49E480);
static const char*& code_pxma = *reinterpret_cast<const char**>(0x49364C);

static MapName& gMapName = *reinterpret_cast<MapName*>(0x49E590);
static RECT& mapname_rect = *reinterpret_cast<RECT*>(0x493650); // Not original name

static auto& gMapping = *reinterpret_cast<signed char(*)[0x80]>(0x49E5B8);

static auto& gTMT = *reinterpret_cast<TRANSMAPTABLE(*)[95]>(0x4937B0);

}