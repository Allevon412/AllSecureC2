#pragma once
#include "agent.h"
#include "localcstd.h"

UINT64 Rotr64(UINT64 value, UINT count);
UINT64 Rotr64HashA(const LPSTR str);
UINT64 Rotr64HashW(const LPWSTR str);
LPVOID RetrieveModuleHandleFromHash(UINT64 hash);
LPVOID RetrieveFunctionPointerFromhash(HMODULE Module, UINT64 Hash);
void printhashes();