// In case you want to call the original vanilla functions(TM) instead of
// your compiler's CRT library functions, for some reason
// (It shouldn't make a difference, honestly...)

#pragma once

namespace csvanilla
{

using FILE = void*;
using size_t = unsigned; // Kind of a lazy way to avoid #include-ing another header

const auto memset = reinterpret_cast<void* (*)(void*, int, size_t)>(0x480D30);
const auto fclose = reinterpret_cast<int(*)(FILE*)>(0x480E1B);
const auto fread = reinterpret_cast<size_t(*)(void*, size_t, size_t, FILE*)>(0x480F55);
const auto fopen = reinterpret_cast<FILE * (*)(const char*, const char*)>(0x480FFD);
const auto sprintf = reinterpret_cast<int(*)(char*, const char*, ...)>(0x481010);
const auto strcmp = reinterpret_cast<int(*)(const char*, const char*)>(0x481070);
const auto strcpy = reinterpret_cast<char* (*)(char*, const char*)>(0x481100);
const auto strcat = reinterpret_cast<char* (*)(char*, const char*)>(0x481110);
const auto strlen = reinterpret_cast<size_t(*)(const char*)>(0x481200);
const auto free = reinterpret_cast<void(*)(void*)>(0x48128B);
const auto malloc = reinterpret_cast<void* (*)(size_t)>(0x4813A3);
const auto memcpy = reinterpret_cast<void* (*)(void*, const void*, size_t)>(0x4813C0);
const auto srand = reinterpret_cast<void(*)(unsigned)>(0x4816FD);
const auto rand = reinterpret_cast<int(*)(void)>(0x48170A);
const auto memcmp = reinterpret_cast<int(*)(const void*, const void*, size_t)>(0x481730);
const auto sscanf = reinterpret_cast<int(*)(const char*, const char*, ...)>(0x4817E8);
const auto fprintf = reinterpret_cast<int(*)(FILE*, const char*, ...)>(0x48181C);
const auto fwrite = reinterpret_cast<int(*)(void*, size_t, size_t, FILE*)>(0x481981);
const auto fseek = reinterpret_cast<int(*)(FILE*, int, int)>(0x481A5C);

}
