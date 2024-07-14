#pragma once
#include <Windows.h>

typedef PIMAGE_RUNTIME_FUNCTION_ENTRY PERF;

PIMAGE_RUNTIME_FUNCTION_ENTRY RTFindFunctionByAddress(UINT64, DWORD64);
PIMAGE_RUNTIME_FUNCTION_ENTRY RTFindFunctionByIndex(UINT64, DWORD);
DWORD GetStackFrameSize(HMODULE, PVOID, DWORD*);
DWORD GetStackFrameSizeWhereRbpIsPushedOnStack(HMODULE, PVOID, DWORD*);
DWORD GetStackFrameSizeIgnoringUwopSetFpreg(HMODULE, PVOID, DWORD*);
void PrintUnwindInfo(HMODULE, PVOID);
void LookupSymbolFromRTIndex(HMODULE, int, BOOL);
void EnumAllRTFunctions(HMODULE);
DWORD FindRTFunctionsUnwind(HMODULE, PVOID);


VOID FindGadget(HMODULE moduleBase, PERF pRuntimeFunctionTable, DWORD rtLastIndex, PDWORD stackSize, PDWORD prtSaveIndex, PDWORD skip, DWORD gadgetType);
DWORD FindProlog(HMODULE moduleBase, PERF pRuntimeFunctionTable, DWORD rtLastIndex, PDWORD stackSize, PDWORD prtSaveIndex, PDWORD skip, PDWORD64 rtTargetOffset);
DWORD FindPushRbp(HMODULE moduleBase, PERF pRuntimeFunctionTable, DWORD rtLastIndex, PDWORD stackSize, PDWORD prtSaveIndex, PDWORD skip, PDWORD64 rtTargetOffset);