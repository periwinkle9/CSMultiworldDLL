#include "patch_utils.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "doukutsu/credits.h"

void applyPostInitPatches();

namespace patcher
{

void patchBytes(dword address, const byte bytes[], unsigned size)
{
	void* addr = reinterpret_cast<void*>(address);
	WriteProcessMemory(GetCurrentProcess(), addr, static_cast<const void*>(bytes), size, NULL);
}

void writeNOPs(dword address, unsigned numBytes)
{
	// Not sure if 'new' calls a CRT function that I'm not supposed to call
	// from within DllMain, but let's just hope this is fine
	// (Note to self: If this doesn't work then do this instead:
	//byte* bytes = static_cast<byte*>(HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, numBytes));
	byte* bytes = new byte[numBytes];
	FillMemory(bytes, numBytes, 0x90); // Fill with NOP
	patchBytes(address, bytes, numBytes);
	//HeapFree(GetProcessHeap(), 0, bytes);
	delete[] bytes;
}


static void postInitHook()
{
	// This function is called in place of InitCreditScript(), so we have to replace the original call here
	csvanilla::InitCreditScript();
	// Then call user-defined code
	applyPostInitPatches();
}
void setupPostInitHook()
{
	// Replaces the call to InitCreditScript() in Game() with our postInitHook() function
	// I chose this location to place the hook for two reasons:
	// 1) So that it doesn't conflict with Clownacy's mod loader (which hooks into the
	//    sprintf call at the end of LoadGenericData()), and
	// 2) It's right after most things in the game have been initialized
	//    (the Mode[Opening/Title/Action] functions are called immediately after this), so
	//    one can potentially do stuff that depends on things having sane values
	//    (not sure what).
	writeCall(0x40F68B, postInitHook);
}

}
