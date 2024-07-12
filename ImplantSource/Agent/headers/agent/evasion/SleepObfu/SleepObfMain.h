#pragma once
#include "../PeParsing/evasive.h"
#include "../../../taskcontroller/TaskController.h"

UINT GetSleepTime();
void PerformSleepObf();
BOOL EventSet(_In_ HANDLE Event);



#define _Ekko

#ifdef _Ekko
#include "EkkoSleepObf.h"
#elif _Zilean
#include "ZileanSleepObf.h"
#elif _FOLIAGE
#include "FoliageSleepObf.h"
#endif