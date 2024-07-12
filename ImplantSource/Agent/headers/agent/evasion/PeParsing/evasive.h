#pragma once
#include "../../agent.h"
#include "../../../cstdreplacement/localcstd.h"

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


#define DEREF( name )*(UINT_PTR *)(name)
#define DEREF_64( name )*(DWORD64 *)(name)
#define DEREF_32( name )*(DWORD *)(name)
#define DEREF_16( name )*(WORD *)(name)
#define DEREF_8( name )*(BYTE *)(name)

PVOID GetExceptionDirectoryAddress(HMODULE hModule, DWORD* tSize);
PVOID GetExportDirectoryAddress(HMODULE hModule);
char* GetSymbolNameByOffset(HMODULE hModule, UINT64 offset);
UINT64 GetSymbolOffset(HMODULE hModule, UINT64 FuncHash);
UINT64 GetSymbolAddress(HMODULE hModule, UINT64 FuncHash);