//
// Created by Brendan Ortiz on 7/19/2024.
//

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "../../agent.h"
#include "../PeParsing/evasive.h"


typedef struct _SYSCALL_ENTRY {
    DWORD64 dw64Hash; //hash of the syscall
    ULONG_PTR uAddress; //address of the syscall to sort the array.
}SYSCALL_ENTRY, *pSYSCALL_ENTRY;

#define MAX_ENTRIES 600

typedef struct _SYS_ENTRY_LIST {
    SYSCALL_ENTRY Entries[MAX_ENTRIES];
    UINT32 u32Count;
}SYS_ENTRY_LIST, *pSYS_ENTRY_LIST;


extern pSYS_ENTRY_LIST g_SyscallList;


#endif //SYSCALLS_H
