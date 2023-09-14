#pragma once

#if !(defined _M_IX86 || defined __i386__)
#error Please compile for x86!
#endif

#include <cstdint>

namespace patcher
{

using byte = std::uint8_t;
using dword = std::uint32_t;

// Checks that the memory starting at the given address matches the expected bytes
// Returns true if the bytes match. Use for verifying that you're not about to write
// a patch that conflicts with an existing ASM hack.
bool verifyBytes(dword address, const byte expectedBytes[], unsigned size);

// Writes the given bytes to the specified address
// (Think "Booster's Lab hex patcher")
void patchBytes(dword address, const byte bytes[], unsigned size);

// Writes a CALL instruction to the given function at the specified address
template <typename Func>
void writeCall(dword address, Func* func)
{
	byte bytes[] = {0xE8, 0x00, 0x00, 0x00, 0x00}; // CALL 0x00000000
	// The argument to CALL is the relative offset from the next instruction
	dword offset = reinterpret_cast<dword>(func) - (address + 5);
	*reinterpret_cast<dword*>(&bytes[1]) = offset; // Replace 0x00000000 with the actual offset
	patchBytes(address, bytes, sizeof bytes);
}

// Replaces the vanilla function located at 'ogFunc' (can be an address or a function pointer) with 'newFunc'
template <typename Func1, typename Func2>
void replaceFunction(Func1 ogFunc, Func2* newFunc)
{
	// Patch the start of 'ogFunc' to redirect it to 'newFunc'
	byte bytes[] = {0x68, 0, 0, 0, 0, // PUSH 0x00000000
					0xC3};            // RET
	*reinterpret_cast<dword*>(&bytes[1]) = reinterpret_cast<dword>(newFunc); // Replace 0x00000000 with the actual address
	patchBytes((dword)(ogFunc), bytes, sizeof bytes);
}

// Writes 'numBytes' of NOP instructions starting at 'address'.
// You know, in case you forgot what the opcode for NOP is (it's 0x90 btw)
void writeNOPs(dword address, unsigned numBytes);



// Call this inside applyPatches() in patches.cpp if you want to do any kind of complicated
// initialization or otherwise something that's unsafe to do within DllMain()
void setupPostInitHook();

// Call this inside applyPatches() in patches.cpp if you have any cleanup routines that should
// be ran when exiting the game (e.g., deallocating memory), but that is unsafe to do during DLL unloading
void setupCleanupHook();

}