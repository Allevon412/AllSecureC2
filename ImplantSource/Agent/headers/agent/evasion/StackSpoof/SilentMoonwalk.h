#pragma once

#include "SilentMoonwalkCommon.h"
#include "Spoof.h"

PVOID SilentMoonwalkMain(PVOID FunctionPointer, PArgs args, PVOID RetAddr);


#define MoonWalk( pFunc, pArgs )                                                       \
    if(1){                                                                             \
        SilentMoonwalkMain((PVOID)pFunc, (PArgs)pArgs, agent->Walker->RetAddr);        \
                                                                                       \
    }