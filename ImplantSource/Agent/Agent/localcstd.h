#pragma once
#include <Windows.h>

#define CopyMemory __builtin_memcpy // using compilers built in memory copy. function.
#define SetMemory __stosb

SIZE_T StringLengthA(LPCSTR String);
SIZE_T StringLengthW(LPCWSTR String);