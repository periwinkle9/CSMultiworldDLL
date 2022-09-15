#pragma once

#include "types.h"

namespace csvanilla
{

// Structures
struct NPCHAR
{
	unsigned char cond;
	int flag;
	int x;
	int y;
	int xm;
	int ym;
	int xm2;
	int ym2;
	int tgt_x;
	int tgt_y;
	int code_char;
	int code_flag;
	int code_event;
	int surf;
	int hit_voice;
	int destroy_voice;
	int life;
	int exp;
	int size;
	int direct;
	unsigned short bits;
	RECT rect;
	int ani_wait;
	int ani_no;
	int count1;
	int count2;
	int act_no;
	int act_wait;
	RANGE hit;
	RANGE view;
	unsigned char shock;
	int damage_view;
	int damage;
	NPCHAR* pNpc;
};
struct EVENTDATA
{
	short x;
	short y;
	short code_flag;
	short code_event;
	short code_char;
	unsigned short bits;
};

// Functions
const auto InitNpChar = reinterpret_cast<void(*)(void)>(0x46EB30);
const auto LoadEvent = reinterpret_cast<BOOL(*)(const char*)>(0x46EB50);
const auto SetUniqueParameter = reinterpret_cast<void(*)(NPCHAR*)>(0x46EE50);
const auto SetNpChar = reinterpret_cast<void(*)(int, int, int, int, int, int, NPCHAR*, int)>(0x46EFD0);
const auto SetDestroyNpChar = reinterpret_cast<void(*)(int, int, int, int)>(0x46F150);
const auto SetDestroyNpCharUp = reinterpret_cast<void(*)(int, int, int, int)>(0x46F200);
const auto SetExpObjects = reinterpret_cast<void(*)(int, int, int)>(0x46F2B0);
const auto SetBulletObject = reinterpret_cast<BOOL(*)(int, int, int)>(0x46F430);
const auto SetLifeObject = reinterpret_cast<BOOL(*)(int, int, int)>(0x46F630);
const auto VanishNpChar = reinterpret_cast<void(*)(NPCHAR*)>(0x46F760);
const auto PutNpChar = reinterpret_cast<void(*)(int, int)>(0x46F810);
const auto ActNpChar = reinterpret_cast<void(*)(void)>(0x46FA00);
const auto ChangeNpCharByEvent = reinterpret_cast<void(*)(int, int, int)>(0x46FAB0);
const auto ChangeCheckableNpCharByEvent = reinterpret_cast<void(*)(int, int, int)>(0x46FD10);
const auto SetNpCharActionNo = reinterpret_cast<void(*)(int, int, int)>(0x46FF90);
const auto MoveNpChar = reinterpret_cast<void(*)(int, int, int, int)>(0x470060);
const auto DeleteNpCharEvent = reinterpret_cast<void(*)(int)>(0x470250);
const auto DeleteNpCharCode = reinterpret_cast<void(*)(int, BOOL)>(0x4702D0);
const auto GetNpCharPosition = reinterpret_cast<void(*)(int*, int*, int)>(0x470460);
const auto IsNpCharCode = reinterpret_cast<BOOL(*)(int)>(0x470490);
const auto GetNpCharAlive = reinterpret_cast<BOOL(*)(int)>(0x4704F0);
const auto CountAliveNpChar = reinterpret_cast<int(*)(void)>(0x470560);

const auto JadgeHitNpCharBlock = reinterpret_cast<void(*)(NPCHAR*, int, int)>(0x4705C0); // Yes, this is misspelled >:)
const auto JudgeHitNpCharTriangleA = reinterpret_cast<void(*)(NPCHAR*, int, int)>(0x470870);
const auto JudgeHitNpCharTriangleB = reinterpret_cast<void(*)(NPCHAR*, int, int)>(0x470970);
const auto JudgeHitNpCharTriangleC = reinterpret_cast<void(*)(NPCHAR*, int, int)>(0x470A70);
const auto JudgeHitNpCharTriangleD = reinterpret_cast<void(*)(NPCHAR*, int, int)>(0x470B70);
const auto JudgeHitNpCharTriangleE = reinterpret_cast<void(*)(NPCHAR*, int, int)>(0x470C70);
const auto JudgeHitNpCharTriangleF = reinterpret_cast<void(*)(NPCHAR*, int, int)>(0x470D80);
const auto JudgeHitNpCharTriangleG = reinterpret_cast<void(*)(NPCHAR*, int, int)>(0x470E90);
const auto JudgeHitNpCharTriangleH = reinterpret_cast<void(*)(NPCHAR*, int, int)>(0x470FA0);
const auto JudgeHitNpCharWater = reinterpret_cast<void(*)(NPCHAR*, int, int)>(0x4710B0);
const auto HitNpCharMap = reinterpret_cast<void(*)(void)>(0x471160);
const auto LoseNpChar = reinterpret_cast<void(*)(NPCHAR*, BOOL)>(0x471B80);
const auto HitNpCharBullet = reinterpret_cast<void(*)(void)>(0x471D50);

// Global variables
static auto& gNPC = *reinterpret_cast<NPCHAR(*)[0x200]>(0x4A6220);
static int& gCurlyShoot_wait = *reinterpret_cast<int*>(0x4BBA2C);
static int& gCurlyShoot_x = *reinterpret_cast<int*>(0x4BBA20);
static int& gCurlyShoot_y = *reinterpret_cast<int*>(0x4BBA24);
static int& gSuperXpos = *reinterpret_cast<int*>(0x4BBA30);
static int& gSuperYpos = *reinterpret_cast<int*>(0x4BBA28);
static const char*& gPassPixEve = *reinterpret_cast<const char**>(0x498540);

}