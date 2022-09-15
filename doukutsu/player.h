#pragma once

#include "types.h"

namespace csvanilla
{

// Structures
struct MYCHAR
{
	unsigned char cond;
	unsigned int flag;
	int direct;
	BOOL up;
	BOOL down;
	int unit;
	int equip;
	int x;
	int y;
	int tgt_x;
	int tgt_y;
	int index_x;
	int index_y;
	int xm;
	int ym;
	int ani_wait;
	int ani_no;
	RANGE hit;
	RANGE view;
	RECT rect;
	RECT rect_arms;
	int level;
	int exp_wait;
	int exp_count;
	unsigned char shock;
	unsigned char no_life;
	unsigned char rensha;
	unsigned char bubble;
	short life;
	short star;
	short max_life;
	short a;
	int lifeBr;
	int lifeBr_count;
	int air;
	int air_get;
	signed char sprash;
	signed char ques;
	signed char boost_sw;
	int boost_cnt;
};
/*
struct COUNTERRECORDE
{
	int counter[4];
	unsigned char random[4];
};
*/

// Functions
const auto InitMyChar = reinterpret_cast<void(*)(void)>(0x414B50);
const auto AnimationMyChar = reinterpret_cast<void(*)(BOOL)>(0x414BF0);
const auto ShowMyChar = reinterpret_cast<void(*)(BOOL)>(0x415220);
const auto PutMyChar = reinterpret_cast<void(*)(int, int)>(0x415250);
const auto ActMyChar_Normal = reinterpret_cast<void(*)(BOOL)>(0x4156C0);
const auto ActMyChar_Stream = reinterpret_cast<void(*)(BOOL)>(0x416470);
const auto ActMyChar = reinterpret_cast<void(*)(BOOL)>(0x4168C0);
const auto AirProcess = reinterpret_cast<void(*)(void)>(0x416990);
const auto GetMyCharPosition = reinterpret_cast<void(*)(int*, int*)>(0x416AA0);
const auto SetMyCharPosition = reinterpret_cast<void(*)(int, int)>(0x416AC0);
const auto MoveMyChar = reinterpret_cast<void(*)(int, int)>(0x416B30);
const auto ZeroMyCharXMove = reinterpret_cast<void(*)(void)>(0x416B50);
const auto GetUnitMyChar = reinterpret_cast<int(*)(void)>(0x416B60);
const auto SetMyCharDirect = reinterpret_cast<void(*)(unsigned char)>(0x416B70);
const auto ChangeMyUnit = reinterpret_cast<void(*)(unsigned char)>(0x416C40);
const auto PitMyChar = reinterpret_cast<void(*)(void)>(0x416C50);
const auto EquipItem = reinterpret_cast<void(*)(int, BOOL)>(0x416C70);
const auto ResetCheck = reinterpret_cast<void(*)(void)>(0x416CA0);

const auto ResetMyCharFlag = reinterpret_cast<void(*)(void)>(0x416E20);
const auto JudgeHitMyCharBlock = reinterpret_cast<int(*)(int, int)>(0x416E30);
const auto PutlittleStar = reinterpret_cast<void(*)(void)>(0x417160);
const auto JudgeHitMyCharTriangleA = reinterpret_cast<int(*)(int, int)>(0x4171D0);
const auto JudgeHitMyCharTriangleB = reinterpret_cast<int(*)(int, int)>(0x4172E0);
const auto JudgeHitMyCharTriangleC = reinterpret_cast<int(*)(int, int)>(0x4173F0);
const auto JudgeHitMyCharTriangleD = reinterpret_cast<int(*)(int, int)>(0x417500);
const auto JudgeHitMyCharTriangleE = reinterpret_cast<int(*)(int, int)>(0x417610);
const auto JudgeHitMyCharTriangleF = reinterpret_cast<int(*)(int, int)>(0x417720);
const auto JudgeHitMyCharTriangleG = reinterpret_cast<int(*)(int, int)>(0x417830);
const auto JudgeHitMyCharTriangleH = reinterpret_cast<int(*)(int, int)>(0x417940);
const auto JudgeHitMyCharWater = reinterpret_cast<int(*)(int, int)>(0x417A50);
const auto JudgeHitMyCharDamage = reinterpret_cast<int(*)(int, int)>(0x417AE0);
const auto JudgeHitMyCharDamageW = reinterpret_cast<int(*)(int, int)>(0x417B70);
const auto JudgeHitMyCharVectLeft = reinterpret_cast<int(*)(int, int)>(0x417C00);
const auto JudgeHitMyCharVectUp = reinterpret_cast<int(*)(int, int)>(0x417C90);
const auto JudgeHitMyCharVectRight = reinterpret_cast<int(*)(int, int)>(0x417D20);
const auto JudgeHitMyCharVectDown = reinterpret_cast<int(*)(int, int)>(0x417DB0);
const auto HitMyCharMap = reinterpret_cast<void(*)(void)>(0x417E40);

struct NPCHAR;
const auto JudgeHitMyCharNPC = reinterpret_cast<int(*)(NPCHAR*)>(0x4187F0);
const auto JudgeHitMyCharNPC3 = reinterpret_cast<unsigned char(*)(NPCHAR*)>(0x418B10);
const auto JudgeHitMyCharNPC4 = reinterpret_cast<int(*)(NPCHAR*)>(0x418C20);
const auto HitMyCharNpChar = reinterpret_cast<void(*)(void)>(0x419030);
const auto HitMyCharBoss = reinterpret_cast<void(*)(void)>(0x419450);

const auto AddExpMyChar = reinterpret_cast<void(*)(int)>(0x4196F0);
const auto ZeroExpMyChar = reinterpret_cast<void(*)(void)>(0x419890);
const auto IsMaxExpMyChar = reinterpret_cast<BOOL(*)(void)>(0x4198C0);
const auto DamageMyChar = reinterpret_cast<void(*)(int)>(0x419910);
const auto ZeroArmsEnergy_All = reinterpret_cast<void(*)(void)>(0x419B50);
const auto AddBulletMyChar = reinterpret_cast<void(*)(int, int)>(0x419BA0);
const auto AddLifeMyChar = reinterpret_cast<void(*)(int)>(0x419C60);
const auto AddMaxLifeMyChar = reinterpret_cast<void(*)(int)>(0x419CB0);
const auto PutArmsEnergy = reinterpret_cast<void(*)(BOOL)>(0x419D10);
const auto PutActiveArmsList = reinterpret_cast<void(*)(void)>(0x41A0B0);
const auto PutMyLife = reinterpret_cast<void(*)(BOOL)>(0x41A1D0);
const auto PutMyAir = reinterpret_cast<void(*)(int, int)>(0x41A350);
const auto PutTimeCounter = reinterpret_cast<void(*)(int, int)>(0x41A430);
const auto SaveTimeCounter = reinterpret_cast<BOOL(*)(void)>(0x41A5D0);
const auto LoadTimeCounter = reinterpret_cast<int(*)(void)>(0x41A7C0);

const auto BackStepMyChar = reinterpret_cast<void(*)(int)>(0x470150);

// Global variables
static MYCHAR& gMC = *reinterpret_cast<MYCHAR*>(0x49E638);
static int& time_count = *reinterpret_cast<int*>(0x49E6F4); // Nikumaru/290 counter value

}