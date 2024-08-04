#pragma once
#include "../agent/agent.h"


#define MemorySet __stosb

SIZE_T StringLengthA(LPCSTR String);
SIZE_T StringLengthW(LPCWSTR String);
LPSTR ToLowerA(LPSTR String);
LPWSTR ToLowerW(LPWSTR String);

void MemoryCopy(void* dest, const void* src, size_t n);
INT MemoryCompare(const void* buf1, const void* buf2, size_t count);
int GetLPSTRArraySize(LPSTR* array);
UINT32 bswap32(UINT32 x);
LPSTR generateRandomString(size_t length);
LPWSTR generateRandomStringW(size_t length);
UINT64 GetUnixTime();
SIZE_T WCharToChar(PCHAR Destination, PWCHAR Source, SIZE_T MaximumAllowed);
