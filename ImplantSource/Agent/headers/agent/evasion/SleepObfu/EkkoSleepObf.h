#pragma once
#include "SleepObfMain.h"
#include <winternl.h>


BOOL EkkoSleepObf(
	_In_ DWORD TimeOut
);

typedef struct _ProtectData {
	DWORD ProtectValue;
	DWORD RegionSize;
}ProtectData, * pProtectData;

void GetVirtualProtections(pProtectData ProtectArr);

#define JMP_RBX 0x1
#define JMP_RAX 0x0

#define SetJmpObfuscation( i, p) \
	if ( JmpBypass == JMP_RBX ) { \
		Rop[i].Rbx = (ULONG_PTR)p; \
	} else { \
		Rop[i].Rax = (ULONG_PTR)p; \
	} \

