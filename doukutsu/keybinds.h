#pragma once

namespace csvanilla
{

// Hacky solution to have scoped enums but without needing to cast to do bitwise operations with them
namespace Key
{
enum
{
	// NOTE: In most cases you should be using the gKey[X] variables below,
	// rather than the KEY_[X] values listed here.
	// (The gKey[X] variables respect the button mappings set in Config.dat,
	// unlike these values.)
	KEY_LEFT      = 0x00000001,
	KEY_RIGHT     = 0x00000002,
	KEY_UP        = 0x00000004,
	KEY_DOWN      = 0x00000008,
	KEY_MAP       = 0x00000010,
	KEY_X         = 0x00000020,
	KEY_Z         = 0x00000040,
	KEY_ARMS      = 0x00000080,
	KEY_ARMSREV   = 0x00000100,
	KEY_SHIFT     = 0x00000200,
	KEY_F1        = 0x00000400,
	KEY_F2        = 0x00000800,
	KEY_ITEM      = 0x00001000,
	KEY_ESCAPE    = 0x00008000,
	KEY_ALT_LEFT  = 0x00010000,
	KEY_ALT_DOWN  = 0x00020000,
	KEY_ALT_RIGHT = 0x00040000,
	KEY_L         = 0x00080000,
	KEY_PLUS      = 0x00100000,
	KEY_ALT_UP    = 0x00180000 // KEY_L + KEY_PLUS
};
}

// Functions
const auto GetTrg = reinterpret_cast<void(*)(void)>(0x4122E0);

// Global variables
static int& gKey = *reinterpret_cast<int*>(0x49E210);
static int& gKeyTrg = *reinterpret_cast<int*>(0x49E214);
static int& gKeyJump = *reinterpret_cast<int*>(0x493610);
static int& gKeyShot = *reinterpret_cast<int*>(0x493614);
static int& gKeyArms = *reinterpret_cast<int*>(0x493618);
static int& gKeyArmsRev = *reinterpret_cast<int*>(0x49361C);
static int& gKeyItem = *reinterpret_cast<int*>(0x493620);
static int& gKeyMap = *reinterpret_cast<int*>(0x493624);
static int& gKeyOk = *reinterpret_cast<int*>(0x493628);
static int& gKeyCancel = *reinterpret_cast<int*>(0x49362C);
static int& gKeyLeft = *reinterpret_cast<int*>(0x493630);
static int& gKeyUp = *reinterpret_cast<int*>(0x493634);
static int& gKeyRight = *reinterpret_cast<int*>(0x493638);
static int& gKeyDown = *reinterpret_cast<int*>(0x49363C);

}