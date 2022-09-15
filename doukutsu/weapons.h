#pragma once

#include "types.h"

namespace csvanilla
{

// Structures
struct BULLET
{
	int flag;
	int code_bullet;
	int bbits;
	int cond;
	int x;
	int y;
	int xm;
	int ym;
	int tgt_x;
	int tgt_y;
	int act_no;
	int act_wait;
	int ani_wait;
	int ani_no;
	unsigned char direct;
	RECT rect;
	int count1;
	int count2;
	int life_count;
	int damage;
	int life;
	int enemyXL;
	int enemyYL;
	int blockXL;
	int blockYL;
	RANGE view;
};

struct BULLETTABLE
{
	signed char damage;
	signed char life;
	int life_count;
	int bbits;
	int enemyXL;
	int enemyYL;
	int blockXL;
	int blockYL;
	RANGE view;
};

// Functions
const auto JudgeHitBulletBlock = reinterpret_cast<int(*)(int, int, BULLET*)>(0x4029B0);
const auto JudgeHitBulletBlock2 = reinterpret_cast<int(*)(int, int, unsigned char*, BULLET*)>(0x402B30);
const auto JudgeHitBulletTriangleA = reinterpret_cast<int(*)(int, int, BULLET*)>(0x402FC0);
const auto JudgeHitBulletTriangleB = reinterpret_cast<int(*)(int, int, BULLET*)>(0x4030B0);
const auto JudgeHitBulletTriangleC = reinterpret_cast<int(*)(int, int, BULLET*)>(0x4031A0);
const auto JudgeHitBulletTriangleD = reinterpret_cast<int(*)(int, int, BULLET*)>(0x403290);
const auto JudgeHitBulletTriangleE = reinterpret_cast<int(*)(int, int, BULLET*)>(0x403380);
const auto JudgeHitBulletTriangleF = reinterpret_cast<int(*)(int, int, BULLET*)>(0x403470);
const auto JudgeHitBulletTriangleG = reinterpret_cast<int(*)(int, int, BULLET*)>(0x403560);
const auto JudgeHitBulletTriangleH = reinterpret_cast<int(*)(int, int, BULLET*)>(0x403650);
const auto HitBulletMap = reinterpret_cast<void(*)(void)>(0x403740);

const auto InitBullet = reinterpret_cast<void(*)(void)>(0x403C00);
const auto CountArmsBullet = reinterpret_cast<int(*)(int)>(0x403C40);
const auto CountBulletNum = reinterpret_cast<int(*)(int)>(0x403CB0);
const auto DeleteBullet = reinterpret_cast<void(*)(int)>(0x403D10);
const auto ClearBullet = reinterpret_cast<void(*)(void)>(0x403D80);
const auto PutBullet = reinterpret_cast<void(*)(int, int)>(0x403DC0);
const auto SetBullet = reinterpret_cast<void(*)(int, int, int, int)>(0x403F80);

const auto ActBullet_Frontia1 = reinterpret_cast<void(*)(BULLET*)>(0x404160);
const auto ActBullet_Frontia2 = reinterpret_cast<void(*)(BULLET*, int)>(0x4043F0);
const auto ActBullet_PoleStar = reinterpret_cast<void(*)(BULLET*, int)>(0x4047B0);
const auto ActBullet_FireBall = reinterpret_cast<void(*)(BULLET*, int)>(0x404B30);
const auto ActBullet_MachineGun = reinterpret_cast<void(*)(BULLET*, int)>(0x405120);
const auto ActBullet_Missile = reinterpret_cast<void(*)(BULLET*, int)>(0x4055A0);
const auto ActBullet_Bom = reinterpret_cast<void(*)(BULLET*, int)>(0x405D80);
const auto ActBullet_Bubblin1 = reinterpret_cast<void(*)(BULLET*)>(0x405F30);
const auto ActBullet_Bubblin2 = reinterpret_cast<void(*)(BULLET*)>(0x406190);
const auto ActBullet_Bubblin3 = reinterpret_cast<void(*)(BULLET*)>(0x4064D0);
const auto ActBullet_Spine = reinterpret_cast<void(*)(BULLET*)>(0x4068B0);
const auto ActBullet_Sword1 = reinterpret_cast<void(*)(BULLET*)>(0x406BB0);
const auto ActBullet_Sword2 = reinterpret_cast<void(*)(BULLET*)>(0x406E60);
const auto ActBullet_Sword3 = reinterpret_cast<void(*)(BULLET*)>(0x407110);
const auto ActBullet_Edge = reinterpret_cast<void(*)(BULLET*)>(0x4075E0);
const auto ActBullet_Drop = reinterpret_cast<void(*)(BULLET*)>(0x4078A0);
const auto ActBullet_SuperMissile = reinterpret_cast<void(*)(BULLET*, int)>(0x407910);
const auto ActBullet_SuperBom = reinterpret_cast<void(*)(BULLET*, int)>(0x408080);
const auto ActBullet_Nemesis = reinterpret_cast<void(*)(BULLET*, int)>(0x408230);
const auto ActBullet_Spur = reinterpret_cast<void(*)(BULLET*, int)>(0x408710);
const auto ActBullet_SpurTail = reinterpret_cast<void(*)(BULLET*, int)>(0x408AE0);
const auto ActBullet_EnemyClear = reinterpret_cast<void(*)(BULLET*)>(0x408F40);
const auto ActBullet_Star = reinterpret_cast<void(*)(BULLET*)>(0x408F90);
const auto ActBullet = reinterpret_cast<void(*)(void)>(0x408FC0);
const auto IsActiveSomeBullet = reinterpret_cast<BOOL(*)(void)>(0x4095C0);

const auto ShootBullet_Frontia1 = reinterpret_cast<void(*)(int)>(0x41DBD0);
const auto ShootBullet_PoleStar = reinterpret_cast<void(*)(int)>(0x41DE60);
const auto ShootBullet_FireBall = reinterpret_cast<void(*)(int)>(0x41E110);
const auto ShootBullet_Machinegun1 = reinterpret_cast<void(*)(int)>(0x41E3D0);
const auto ShootBullet_Missile = reinterpret_cast<void(*)(int, BOOL)>(0x41E7B0);
const auto ShootBullet_Bubblin1 = reinterpret_cast<void(*)(void)>(0x41EFD0);
const auto ShootBullet_Bubblin2 = reinterpret_cast<void(*)(int)>(0x41F280);
const auto ShootBullet_Sword = reinterpret_cast<void(*)(int)>(0x41F580);
const auto ShootBullet_Nemesis = reinterpret_cast<void(*)(int)>(0x41F710);
const auto ResetSpurCharge = reinterpret_cast<void(*)(void)>(0x41F9E0);
const auto ShootBullet_Spur = reinterpret_cast<void(*)(int)>(0x41FA10);
const auto ShootBullet = reinterpret_cast<void(*)(void)>(0x41FE70);

// Global variables
static auto& gBul = *reinterpret_cast<BULLET(*)[64]>(0x499C98);
static auto& gBulTbl = *reinterpret_cast<BULLETTABLE(*)[46]>(0x48F044);

static int& spur_charge = *reinterpret_cast<int*>(0x4A5550);

}