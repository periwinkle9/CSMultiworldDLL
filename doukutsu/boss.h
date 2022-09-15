#pragma once

#include "npc.h"

namespace csvanilla
{

// Structures
struct BOSSLIFE
{
	BOOL flag;
	int* pLife;
	int max;
	int br;
	int count;
};
typedef void (*BOSSFUNCTION)(void);

// Functions
const auto InitBossChar = reinterpret_cast<void(*)(int)>(0x472740);
const auto PutBossChar = reinterpret_cast<void(*)(int, int)>(0x472770);
const auto SetBossCharActNo = reinterpret_cast<void(*)(int)>(0x472940);
const auto HitBossBullet = reinterpret_cast<void(*)(void)>(0x472950);
const auto ActBossChar_0 = reinterpret_cast<void(*)(void)>(0x472FF0);
const auto ActBossChar = reinterpret_cast<void(*)(void)>(0x473000);
const auto HitBossMap = reinterpret_cast<void(*)(void)>(0x473080);

const auto ActBossChar_Core_Face = reinterpret_cast<void(*)(NPCHAR*)>(0x4739B0);
const auto ActBossChar_Core_Tail = reinterpret_cast<void(*)(NPCHAR*)>(0x473BD0);
const auto ActBossChar_Core_Mini = reinterpret_cast<void(*)(NPCHAR*)>(0x473DE0);
const auto ActBossChar_Core_Hit = reinterpret_cast<void(*)(NPCHAR*)>(0x474340);
const auto ActBossChar_Core = reinterpret_cast<void(*)(void)>(0x474400);

const auto ActBossChar_Undead = reinterpret_cast<void(*)(void)>(0x4753D0);
const auto ActBossCharA_Head = reinterpret_cast<void(*)(NPCHAR*)>(0x476790);
const auto ActBossCharA_Tail = reinterpret_cast<void(*)(NPCHAR*)>(0x4769A0);
const auto ActBossCharA_Face = reinterpret_cast<void(*)(NPCHAR*)>(0x476B90);
const auto ActBossCharA_Mini = reinterpret_cast<void(*)(NPCHAR*)>(0x476E50);
const auto ActBossCharA_Hit = reinterpret_cast<void(*)(NPCHAR*)>(0x477230);

const auto ActBossChar_Ballos = reinterpret_cast<void(*)(void)>(0x4772F0);
const auto ActBossChar_Eye = reinterpret_cast<void(*)(NPCHAR*)>(0x478AA0);
const auto ActBossChar_Body = reinterpret_cast<void(*)(NPCHAR*)>(0x478F20);
const auto ActBossChar_HITAI = reinterpret_cast<void(*)(NPCHAR*)>(0x478FE0);
const auto ActBossChar_HARA = reinterpret_cast<void(*)(NPCHAR*)>(0x479010);

const auto ActBossChar_Frog = reinterpret_cast<void(*)(void)>(0x479030);
const auto ActBossChar02_01 = reinterpret_cast<void(*)(void)>(0x47A6A0);
const auto ActBossChar02_02 = reinterpret_cast<void(*)(void)>(0x47A800);

const auto ActBossChar_Ironhead = reinterpret_cast<void(*)(void)>(0x47A8A0);

const auto ActBossChar_Omega = reinterpret_cast<void(*)(void)>(0x47B6F0);
const auto ActBoss01_12 = reinterpret_cast<void(*)(void)>(0x47C380);
const auto ActBoss01_34 = reinterpret_cast<void(*)(void)>(0x47C4E0);
const auto ActBoss01_5 = reinterpret_cast<void(*)(void)>(0x47C7A0);

const auto ActBossChar_Press = reinterpret_cast<void(*)(void)>(0x47C820);

const auto ActBossChar_Twin = reinterpret_cast<void(*)(void)>(0x47D170);
const auto ActBossCharT_DragonBody = reinterpret_cast<void(*)(NPCHAR*)>(0x47DAA0);
const auto ActBossCharT_DragonHead = reinterpret_cast<void(*)(NPCHAR*)>(0x47DF10);

const auto ActBossChar_MonstX = reinterpret_cast<void(*)(void)>(0x47E6F0);
const auto ActBossChar03_01 = reinterpret_cast<void(*)(NPCHAR*)>(0x47F710);
const auto ActBossChar03_02 = reinterpret_cast<void(*)(NPCHAR*)>(0x480090);
const auto ActBossChar03_03 = reinterpret_cast<void(*)(NPCHAR*)>(0x4802A0);
const auto ActBossChar03_04 = reinterpret_cast<void(*)(NPCHAR*)>(0x480550);
const auto ActBossChar03_face = reinterpret_cast<void(*)(NPCHAR*)>(0x4808C0);

const auto InitBossLife = reinterpret_cast<void(*)(void)>(0x47B450);
const auto StartBossLife = reinterpret_cast<BOOL(*)(int)>(0x47B460);
const auto StartBossLife2 = reinterpret_cast<BOOL(*)(void)>(0x47B500);
const auto PutBossLife = reinterpret_cast<void(*)(void)>(0x47B540);

// Global variables
static auto& gBoss = *reinterpret_cast<NPCHAR(*)[20]>(0x4BBA58);
static auto& gpBossFuncTbl = *reinterpret_cast<BOSSFUNCTION(*)[10]>(0x498AEC);

static BOSSLIFE& gBL = *reinterpret_cast<BOSSLIFE*>(0x4BBA44);

}