#pragma once

#include "types.h"

namespace csvanilla
{

// Structures
struct CARET
{
	int cond;
	int code;
	int direct;
	int x;
	int y;
	int xm;
	int ym;
	int act_no;
	int act_wait;
	int ani_no;
	int ani_wait;
	int view_left;
	int view_top;
	RECT rect;
};

struct CARETTABLE
{
	int view_left;
	int view_top;
};
typedef void (*CARETFUNCTION)(CARET*);

// Functions
const auto InitCaret = reinterpret_cast<void(*)(void)>(0x409650);
const CARETFUNCTION ActCaretFn[18] = {
	reinterpret_cast<CARETFUNCTION>(0x409670), // 00
	reinterpret_cast<CARETFUNCTION>(0x409680), // 01
	reinterpret_cast<CARETFUNCTION>(0x409880), // 02
	reinterpret_cast<CARETFUNCTION>(0x409B80), // 03
	reinterpret_cast<CARETFUNCTION>(0x409C70), // 04
	reinterpret_cast<CARETFUNCTION>(0x409E00), // 05
	nullptr,                                   // 06 (nonexistent)
	reinterpret_cast<CARETFUNCTION>(0x409F60), // 07
	reinterpret_cast<CARETFUNCTION>(0x40A120), // 08
	reinterpret_cast<CARETFUNCTION>(0x40A1B0), // 09
	reinterpret_cast<CARETFUNCTION>(0x40A280), // 10
	reinterpret_cast<CARETFUNCTION>(0x40A3F0), // 11
	reinterpret_cast<CARETFUNCTION>(0x40A5A0), // 12
	reinterpret_cast<CARETFUNCTION>(0x40A650), // 13
	reinterpret_cast<CARETFUNCTION>(0x40A7E0), // 14
	reinterpret_cast<CARETFUNCTION>(0x40A8F0), // 15
	reinterpret_cast<CARETFUNCTION>(0x40A9E0), // 16
	reinterpret_cast<CARETFUNCTION>(0x40AAA0)  // 17
};
const auto ActCaret = reinterpret_cast<void(*)(void)>(0x40AB50);
const auto PutCaret = reinterpret_cast<void(*)(int, int)>(0x40ABC0);
const auto SetCaret = reinterpret_cast<void(*)(int, int, int, int)>(0x40AC90);

// Whimsical Stars are just CARETs
const auto InitStar = reinterpret_cast<void(*)(void)>(0x420FA0);
const auto ActStar = reinterpret_cast<void(*)(void)>(0x421040);
const auto PutStar = reinterpret_cast<void(*)(int, int)>(0x4213B0);

// Global variables
static auto gCaretTable = *reinterpret_cast<CARETTABLE(*)[18]>(0x48F830);
static auto& gpCaretFuncTbl = *reinterpret_cast<CARETFUNCTION(*)[18]>(0x48F8C0);

static auto& star = *reinterpret_cast<CARET(*)[3]>(0x4A5800);

}