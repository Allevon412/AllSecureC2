#pragma once
#include <Windows.h>


#define MemorySet __stosb

SIZE_T StringLengthA(LPCSTR String);
SIZE_T StringLengthW(LPCWSTR String);

void MemoryCopy(void* dest, const void* src, size_t n);
int GetLPSTRArraySize(LPSTR* array);
UINT32 bswap32(UINT32 x);