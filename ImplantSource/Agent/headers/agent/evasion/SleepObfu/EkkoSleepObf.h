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

