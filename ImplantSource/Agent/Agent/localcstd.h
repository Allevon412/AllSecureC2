#pragma once
#include "agent.h"


#define MemorySet __stosb

SIZE_T StringLengthA(LPCSTR String);
SIZE_T StringLengthW(LPCWSTR String);
LPSTR ToLowerA(LPSTR String);
LPWSTR ToLowerW(LPWSTR String);

void MemoryCopy(void* dest, const void* src, size_t n);
int GetLPSTRArraySize(LPSTR* array);
UINT32 bswap32(UINT32 x);
LPSTR generateRandomString(size_t length);
LPWSTR generateRandomStringW(size_t length);
UINT64 GetUnixTime();
