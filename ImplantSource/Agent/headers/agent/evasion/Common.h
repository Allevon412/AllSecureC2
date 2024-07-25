//
// Created by Brendan Ortiz on 7/25/2024.
//

#ifndef COMMON_H
#define COMMON_H

#include "PeParsing/evasive.h"
#include "SleepObfu/SleepObfMain.h"
#include "StackSpoof/SilentMoonwalk.h"
#include "StackSpoof/Functions.h"
#include "syscalls/syscalls.h"


#define Spoof_X( pFunc, Nargs)                         SpoofStack(pFunc, Nargs, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)
#define Spoof_A( pFunc, Nargs, a)                      SpoofStack(pFunc, Nargs, a,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)
#define Spoof_B( pFunc, Nargs, a,b)                    SpoofStack(pFunc, Nargs, a,b,NULL,NULL,NULL,NULL,NULL,NULL,NULL)
#define Spoof_C( pFunc, Nargs, a,b,c)                  SpoofStack(pFunc, Nargs, a,b,c,NULL,NULL,NULL,NULL,NULL,NULL)
#define Spoof_D( pFunc, Nargs, a,b,c,d)                SpoofStack(pFunc, Nargs, a,b,c,d,NULL,NULL,NULL,NULL,NULL)
#define Spoof_E( pFunc, Nargs, a,b,c,d,e)              SpoofStack(pFunc, Nargs, a,b,c,d,e,NULL,NULL,NULL,NULL)
#define Spoof_F( pFunc, Nargs, a,b,c,d,e,f)            SpoofStack(pFunc, Nargs, a,b,c,d,e,f,NULL,NULL,NULL)
#define Spoof_G( pFunc, Nargs, a,b,c,d,e,f,g)          SpoofStack(pFunc, Nargs, a,b,c,d,e,f,g,NULL,NULL)
#define Spoof_H( pFunc, Nargs, a,b,c,d,e,f,g,h)        SpoofStack(pFunc, Nargs, a,b,c,d,e,f,g,h,NULL)
#define Spoof_J( pFunc, Nargs, a,b,c,d,e,f,g,h,j)      SpoofStack(pFunc, Nargs, a,b,c,d,e,f,g,h,j)
#define SETUP_ARGS(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, ...) _12
#define SPOOF_MACRO_CHOOSER(...) SETUP_ARGS(__VA_ARGS__, Spoof_J, Spoof_H, Spoof_G, Spoof_F, Spoof_E, Spoof_D, Spoof_C, Spoof_B, Spoof_A, Spoof_X, )
#define SpoofStack(...) SPOOF_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

PVOID SpoofStack(
    _In_ PVOID pFunction,
    _In_ INT Nargs,
    _Inout_ PVOID a,
    _Inout_ PVOID b,
    _Inout_ PVOID c,
    _Inout_ PVOID d,
    _Inout_ PVOID e,
    _Inout_ PVOID f,
    _Inout_ PVOID g,
    _Inout_ PVOID h,
    _Inout_ PVOID j
   );
#endif //COMMON_H
