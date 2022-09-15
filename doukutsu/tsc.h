#pragma once

#include "types.h"

namespace csvanilla
{

// Structures
struct TEXTSCRIPT
{
	char path[260];
	int size;
	char* data;
	signed char mode;
	signed char flags;
	int p_read;
	int p_write;
	int line;
	int ypos_line[4];
	int wait;
	int wait_next;
	int next_event;
	signed char select;
	int face;
	int face_x;
	int item;
	int item_y;
	RECT rcText;
	int offsetY;
	unsigned char wait_beam;
};

// Functions
const auto InitTextScript2 = reinterpret_cast<BOOL(*)(void)>(0x4214E0);
const auto EndTextScript = reinterpret_cast<void(*)(void)>(0x421570);
const auto EncryptionBinaryData2 = reinterpret_cast<void(*)(unsigned char*, int)>(0x4215C0);
const auto LoadTextScript2 = reinterpret_cast<BOOL(*)(const char*)>(0x421660);
const auto LoadTextScript_Stage = reinterpret_cast<BOOL(*)(const char*)>(0x421750);
const auto GetTextScriptPath = reinterpret_cast<void(*)(char*)>(0x4218E0);
const auto GetTextScriptNo = reinterpret_cast<int(*)(int)>(0x421900);
const auto StartTextScript = reinterpret_cast<BOOL(*)(int)>(0x421990);
const auto JumpTextScript = reinterpret_cast<BOOL(*)(int)>(0x421AF0);
const auto StopTextScript = reinterpret_cast<void(*)(void)>(0x421C50);
const auto CheckNewLine = reinterpret_cast<void(*)(void)>(0x421C80);
const auto SetNumberTextScript = reinterpret_cast<void(*)(int)>(0x421D10);
const auto ClearTextLine = reinterpret_cast<void(*)(void)>(0x421E90);
const auto PutTextScript = reinterpret_cast<void(*)(void)>(0x421F10);
const auto TextScriptProc = reinterpret_cast<int(*)(void)>(0x422510);
const auto RestoreTextScript = reinterpret_cast<void(*)(void)>(0x425790);

// Global variables
static TEXTSCRIPT& gTS = *reinterpret_cast<TEXTSCRIPT*>(0x4A59D0);
static auto& text = *reinterpret_cast<char(*)[4][0x40]>(0x4A58D0);
static RECT& gRect_line = *reinterpret_cast<RECT*>(0x498290);
static auto& gNumberTextScript = *reinterpret_cast<int(*)[4]>(0x4A5B34);

}