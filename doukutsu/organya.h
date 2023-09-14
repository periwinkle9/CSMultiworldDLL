#pragma once

#ifndef DIRECTSOUND_VERSION
#define DIRECTSOUND_VERSION 0x500
#endif
#include <dsound.h>

namespace csvanilla
{

// Structures
struct ORGANYATRACK
{
	unsigned short freq;
	unsigned char wave_no;
	unsigned char pipi;
	unsigned short note_num;
};
struct ORGANYADATA
{
	unsigned short wait;
	unsigned char line;
	unsigned char dot;
	long repeat_x;
	long end_x;
	ORGANYATRACK tdata[16];
};
struct NOTELIST
{
	NOTELIST* from;
	NOTELIST* to;
	long x;
	unsigned char length;
	unsigned char y;
	unsigned char volume;
	unsigned char pan;
};
struct TRACKDATA
{
	unsigned short freq;
	unsigned char wave_no;
	signed char pipi;
	NOTELIST* note_p;
	NOTELIST* note_list;
};
struct MUSICINFO
{
	unsigned short wait;
	unsigned char line;
	unsigned char dot;
	unsigned short alloc_note;
	long repeat_x;
	long end_x;
	TRACKDATA tdata[16];
};
struct ORGDATA
{
	MUSICINFO info;
	char track;
	char mute[16];
	unsigned char def_pan;
	unsigned char def_volume;
	
	ORGDATA();
	void InitOrgData();
	void GetMusicInfo(MUSICINFO* mi);
	BOOL SetMusicInfo(MUSICINFO* mi, unsigned long flag);
	BOOL NoteAlloc(unsigned short note_num);
	void ReleaseNote(void);
	void PlayData(void);
	void SetPlayPointer(long x);
	BOOL InitMusicData(const char* path);
};
struct OCTWAVE
{
	short wave_size;
	short oct_par;
	short oct_size;
};

// Functions
const auto MakeSoundObject8 = reinterpret_cast<BOOL(*)(signed char*, signed char, signed char)>(0x41A8F0);
const auto ChangeOrganFrequency = reinterpret_cast<void(*)(unsigned char, signed char, long)>(0x41ABA0);
const auto ChangeOrganPan = reinterpret_cast<void(*)(unsigned char, unsigned char, signed char)>(0x41AC70);
const auto ChangeOrganVolume = reinterpret_cast<void(*)(int, long, signed char)>(0x41AD20);
const auto PlayOrganObject = reinterpret_cast<void(*)(unsigned char, int, signed char, long)>(0x41ADC0);
const auto ReleaseOrganyaObject = reinterpret_cast<void(*)(signed char)>(0x41B2A0);
const auto InitWaveData100 = reinterpret_cast<BOOL(*)(void)>(0x41B380);
const auto MakeOrganyaWave = reinterpret_cast<BOOL(*)(signed char, signed char, signed char)>(0x41B3F0);
const auto ChangeDramFrequency = reinterpret_cast<void(*)(unsigned char, signed char)>(0x41B440);
const auto ChangeDramPan = reinterpret_cast<void(*)(unsigned char, signed char)>(0x41B480);
const auto ChangeDramVolume = reinterpret_cast<void(*)(long, signed char)>(0x41B4D0);
const auto PlayDramObject = reinterpret_cast<void(*)(unsigned char, int, signed char)>(0x41B510);
// Pointers to ORGDATA member functions
namespace OrgData
{
const auto ctor = reinterpret_cast<ORGDATA * (__thiscall*)(ORGDATA*)>(0x41B600);
const auto InitOrgData = reinterpret_cast<void(__thiscall*)(ORGDATA*)>(0x41B650);
const auto SetMusicInfo = reinterpret_cast<BOOL(__thiscall*)(ORGDATA*, MUSICINFO*, unsigned long)>(0x41B730);
const auto NoteAlloc = reinterpret_cast<BOOL(__thiscall*)(ORGDATA*, unsigned short)>(0x41B890);
const auto ReleaseNote = reinterpret_cast<void(__thiscall*)(ORGDATA*)>(0x41BA70);
const auto InitMusicData = reinterpret_cast<BOOL(__thiscall*)(ORGDATA*, const char*)>(0x41BAD0);
const auto PlayData = reinterpret_cast<void(__thiscall*)(ORGDATA*)>(0x41C2B0);
const auto SetPlayPointer = reinterpret_cast<void(__thiscall*)(ORGDATA*, long)>(0x41C630);
const auto GetMusicInfo = reinterpret_cast<void(__thiscall*)(ORGDATA*, MUSICINFO*)>(0x41C0B0);
}
// Wrappers for the above in case you need to call them from within your own code
inline ORGDATA::ORGDATA() : info{}, track{}, mute{}, def_pan{}, def_volume{} // Initialize to shut up compiler warnings
{
	OrgData::ctor(this);
}
inline void ORGDATA::InitOrgData()
{
	return OrgData::InitOrgData(this);
}
inline BOOL ORGDATA::SetMusicInfo(MUSICINFO* mi, unsigned long flag)
{
	return OrgData::SetMusicInfo(this, mi, flag);
}
inline BOOL ORGDATA::NoteAlloc(unsigned short note_num)
{
	return OrgData::NoteAlloc(this, note_num);
}
inline void ORGDATA::ReleaseNote(void)
{
	return OrgData::ReleaseNote(this);
}
inline BOOL ORGDATA::InitMusicData(const char* path)
{
	return OrgData::InitMusicData(this, path);
}
inline void ORGDATA::GetMusicInfo(MUSICINFO* mi)
{
	return OrgData::GetMusicInfo(this, mi);
}
inline void ORGDATA::PlayData(void)
{
	return OrgData::PlayData(this);
}
inline void ORGDATA::SetPlayPointer(long x)
{
	return OrgData::SetPlayPointer(this, x);
}
const auto InitMMTimer = reinterpret_cast<BOOL(*)(void)>(0x41C180);
const auto StartTimer = reinterpret_cast<BOOL(*)(DWORD)>(0x41C1E0);
const auto TimerProc = reinterpret_cast<VOID(CALLBACK*)(UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR)>(0x41C230);
const auto QuitMMTimer = reinterpret_cast<BOOL(*)(void)>(0x41C250);
const auto StartOrganya = reinterpret_cast<BOOL(*)(LPDIRECTSOUND, const char*)>(0x41C6C0);
const auto LoadOrganya = reinterpret_cast<BOOL(*)(const char*)>(0x41C6F0);
const auto SetOrganyaPosition = reinterpret_cast<void(*)(unsigned int)>(0x41C730);
const auto GetOrganyaPosition = reinterpret_cast<unsigned int(*)(void)>(0x41C770);
const auto PlayOrganyaMusic = reinterpret_cast<void(*)(void)>(0x41C790);
const auto ChangeOrganyaVolume = reinterpret_cast<BOOL(*)(int)>(0x41C7C0);
const auto StopOrganyaMusic = reinterpret_cast<void(*)(void)>(0x41C7F0);
const auto SetOrganyaFadeout = reinterpret_cast<void(*)(void)>(0x41C880);
const auto EndOrganya = reinterpret_cast<void(*)(void)>(0x41C890);

// Global variables
// I've omitted some stuff that doesn't seem to be particularly useful to tamper with in a mod
// (but let me know if I'm wrong about that)
static auto& lpORGANBUFFER = *reinterpret_cast<LPDIRECTSOUNDBUFFER(*)[8][8][2]>(0x4A4B48);
static auto& oct_wave = *reinterpret_cast<OCTWAVE(*)[8]>(0x493708);
static WAVEFORMATEX& format_tbl2 = *reinterpret_cast<WAVEFORMATEX*>(0x493738);
static auto& freq_tbl = *reinterpret_cast<short(*)[12]>(0x49374C);
static auto& pan_tbl = *reinterpret_cast<short(*)[13]>(0x493764);
static auto& g_mute = *reinterpret_cast<BOOL(*)[16]>(0x4A4D48);
static auto& old_key = *reinterpret_cast<unsigned char(*)[16]>(0x493780);
static auto& key_on = *reinterpret_cast<unsigned char(*)[16]>(0x4A4D88);
static auto& key_twin = *reinterpret_cast<unsigned char(*)[16]>(0x4A4D98);
static auto& wave_data = *reinterpret_cast<signed char(*)[100][0x100]>(0x49E700);
static ORGDATA& org_data = *reinterpret_cast<ORGDATA*>(0x4A4E18);
static long& PlayPos = *reinterpret_cast<LONG*>(0x4A4B00);
static auto& np = *reinterpret_cast<NOTELIST * (*)[16]>(0x4A4B08);
static auto& now_leng = *reinterpret_cast<long(*)[8]>(0x4A4DB0);
static int& Volume = *reinterpret_cast<int*>(0x4937A4);
static auto& TrackVol = *reinterpret_cast<int(*)[16]>(0x4A4DD0);
static BOOL& bFadeout = *reinterpret_cast<BOOL*>(0x4A4E10);

}