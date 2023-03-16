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
	// I could do this more simply with VirtualProtect + FillMemory, but let's not bother with that
	const byte NOPs[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
	
	for (unsigned size = sizeof NOPs; size > 0; size /= 2)
	{
		while (numBytes >= size)
		{
			patchBytes(address, NOPs, size);
			numBytes -= size;
			address += size;
		}
	}
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
