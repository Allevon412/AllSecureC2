#pragma once
#include "agent.h"
#include "localcstd.h"

UINT64 Rotr64(UINT64 value, UINT count);
UINT64 Rotr64HashA(const LPSTR str);
UINT64 Rotr64HashW(const LPWSTR str);
LPVOID RetrieveModuleHandleFromHash(UINT64 hash);
LPVOID RetrieveFunctionPointerFromhash(HMODULE Module, UINT64 Hash);

//returns the base address of a module, takes as input a 8byte hash of the module's name.
// accepts 0 to return the current image's hash.
PVOID GetModuleBaseAddr(_In_ UINT64 Hash);
DWORD GetImageSize(_In_ PVOID ModuleBase);
WORD GetNumberOfSections(_In_ PVOID ModuleBase);
void printhashes();