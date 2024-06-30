#pragma once
#include "evasive.h"
#include "TaskController.h"

UINT GetSleepTime();
void PerformSleepObf();


#define _Ekko

#ifdef _Ekko
#include "EkkoSleepObf.h"
#elif _Zilean
#include "ZileanSleepObf.h"
#elif _FOLIAGE
#include "FoliageSleepObf.h"
#endif