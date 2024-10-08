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


#define Spoof_X( pFunc, Nargs)                         SpoofStackFunc(pFunc, Nargs, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)
#define Spoof_A( pFunc, Nargs, a)                      SpoofStackFunc(pFunc, Nargs, a,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, NULL)
#define Spoof_B( pFunc, Nargs, a,b)                    SpoofStackFunc(pFunc, Nargs, a,b,NULL,NULL,NULL,NULL,NULL,NULL,NULL, NULL)
#define Spoof_C( pFunc, Nargs, a,b,c)                  SpoofStackFunc(pFunc, Nargs, a,b,c,NULL,NULL,NULL,NULL,NULL,NULL, NULL)
#define Spoof_D( pFunc, Nargs, a,b,c,d)                SpoofStackFunc(pFunc, Nargs, a,b,c,d,NULL,NULL,NULL,NULL,NULL, NULL)
#define Spoof_E( pFunc, Nargs, a,b,c,d,e)              SpoofStackFunc(pFunc, Nargs, a,b,c,d,e,NULL,NULL,NULL,NULL, NULL)
#define Spoof_F( pFunc, Nargs, a,b,c,d,e,f)            SpoofStackFunc(pFunc, Nargs, a,b,c,d,e,f,NULL,NULL,NULL, NULL)
#define Spoof_G( pFunc, Nargs, a,b,c,d,e,f,g)          SpoofStackFunc(pFunc, Nargs, a,b,c,d,e,f,g,NULL,NULL, NULL)
#define Spoof_H( pFunc, Nargs, a,b,c,d,e,f,g,h)        SpoofStackFunc(pFunc, Nargs, a,b,c,d,e,f,g,h,NULL, NULL)
#define Spoof_I( pFunc, Nargs, a,b,c,d,e,f,g,h,i)      SpoofStackFunc(pFunc, Nargs, a,b,c,d,e,f,g,h,i, NULL)
#define Spoof_J( pFunc, Nargs, a,b,c,d,e,f,g,h,i,j)    SpoofStackFunc(pFunc, Nargs, a,b,c,d,e,f,g,h,i,j)
#define SETUP_ARGS(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12,_13, ...) _13
#define SPOOF_MACRO_CHOOSER(...) SETUP_ARGS(__VA_ARGS__,Spoof_J, Spoof_I, Spoof_H, Spoof_G, Spoof_F, Spoof_E, Spoof_D, Spoof_C, Spoof_B, Spoof_A, Spoof_X)
#define SpoofStack(...) SPOOF_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)


#define TAMPER_SYSCALL_X( dw64Hash, Nargs)                          TemperSyscallAndSpoofStack(dw64Hash, Nargs, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define TAMPER_SYSCALL_A( dw64Hash, Nargs, a)                       TemperSyscallAndSpoofStack(dw64Hash, Nargs, a, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define TAMPER_SYSCALL_B( dw64Hash, Nargs, a,b)                     TemperSyscallAndSpoofStack(dw64Hash, Nargs, a,b, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define TAMPER_SYSCALL_C( dw64Hash, Nargs, a,b,c)                   TemperSyscallAndSpoofStack(dw64Hash, Nargs, a,b,c, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define TAMPER_SYSCALL_D( dw64Hash, Nargs, a,b,c,d)                 TemperSyscallAndSpoofStack(dw64Hash, Nargs, a,b,c,d, NULL, NULL, NULL, NULL, NULL,NULL, NULL)
#define TAMPER_SYSCALL_E( dw64Hash, Nargs, a,b,c,d,e)               TemperSyscallAndSpoofStack(dw64Hash, Nargs, a,b,c,d,e, NULL, NULL, NULL, NULL,NULL, NULL)
#define TAMPER_SYSCALL_F( dw64Hash, Nargs, a,b,c,d,e,f)             TemperSyscallAndSpoofStack(dw64Hash, Nargs, a,b,c,d,e,f, NULL, NULL, NULL,NULL, NULL)
#define TAMPER_SYSCALL_G( dw64Hash, Nargs, a,b,c,d,e,f,g)           TemperSyscallAndSpoofStack(dw64Hash, Nargs, a,b,c,d,e,f,g, NULL, NULL,NULL, NULL)
#define TAMPER_SYSCALL_H( dw64Hash, Nargs, a,b,c,d,e,f,g,h)         TemperSyscallAndSpoofStack(dw64Hash, Nargs, a,b,c,d,e,f,g,h, NULL,NULL, NULL)
#define TAMPER_SYSCALL_I( dw64Hash, Nargs, a,b,c,d,e,f,g,h,i)       TemperSyscallAndSpoofStack(dw64Hash, Nargs, a,b,c,d,e,f,g,h,i, NULL, NULL)
#define TAMPER_SYSCALL_J( dw64Hash, Nargs, a,b,c,d,e,f,g,h,i,j)     TemperSyscallAndSpoofStack(dw64Hash, Nargs, a,b,c,d,e,f,g,h,i,j, NULL)
#define TAMPER_SYSCALL_K( dw64Hash, Nargs, a,b,c,d,e,f,g,h,i,j,k)   TemperSyscallAndSpoofStack(dw64Hash, Nargs, a,b,c,d,e,f,g,h,i,j,k)
#define SYS_SETUP_ARGS(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12,_13,_14, ...) _14
#define SYS_CALL_MACRO_CHOOSER(...) SYS_SETUP_ARGS(__VA_ARGS__, TAMPER_SYSCALL_K, TAMPER_SYSCALL_J, TAMPER_SYSCALL_I, TAMPER_SYSCALL_H, TAMPER_SYSCALL_G, TAMPER_SYSCALL_F, TAMPER_SYSCALL_E, TAMPER_SYSCALL_D, TAMPER_SYSCALL_C, TAMPER_SYSCALL_B, TAMPER_SYSCALL_A, TAMPER_SYSCALL_X)
#define TAMPER_SYSCALL(...) SYS_CALL_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

PVOID SpoofStackFunc(
    _In_ PVOID pFunction,
    _In_ INT Nargs,
    _Inout_ ULONG_PTR a,
    _Inout_ ULONG_PTR b,
    _Inout_ ULONG_PTR c,
    _Inout_ ULONG_PTR d,
    _Inout_ ULONG_PTR e,
    _Inout_ ULONG_PTR f,
    _Inout_ ULONG_PTR g,
    _Inout_ ULONG_PTR h,
    _Inout_ ULONG_PTR i,
    _Inout_ ULONG_PTR j
   );

NTSTATUS TemperSyscallAndSpoofStack(
    ULONG_PTR uAddress,
    UINT Nargs,
    ULONG_PTR a,
    ULONG_PTR b,
    ULONG_PTR c,
    ULONG_PTR d,
    ULONG_PTR e,
    ULONG_PTR f,
    ULONG_PTR g,
    ULONG_PTR h,
    ULONG_PTR i,
    ULONG_PTR j,
    ULONG_PTR k
    );

PVOID FindJmpGadget( IN PVOID LOC, IN SIZE_T Len, IN PVOID Pattern, IN SIZE_T PatLen );

#endif //COMMON_H