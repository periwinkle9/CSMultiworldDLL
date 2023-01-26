#include "patch_utils.h"
#include "windows_h_wrapper.h"
#include "doukutsu/credits.h"

void applyPostInitPatches();
void cleanup();

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
	// from within DllMain, but let's just play it safe and not use it
	byte* bytes = static_cast<byte*>(HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, numBytes));
	//byte* bytes = new byte[numBytes];
	FillMemory(bytes, numBytes, 0x90); // Fill with NOP
	patchBytes(address, bytes, numBytes);
	HeapFree(GetProcessHeap(), 0, bytes);
	//delete[] bytes;
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

static void cleanupHook()
{
	// This function is called in place of ReleaseCreditScript(), so we replace the original call here
	csvanilla::ReleaseCreditScript();
	// Then call user-defined code
	cleanup();
}
void setupCleanupHook()
{
	// Replace the call to ReleaseCreditScript() in Game() with our cleanupHook() function
	// (This is pretty much right before the game exits)
	writeCall(0x40F6F9, cleanupHook);
}

}
