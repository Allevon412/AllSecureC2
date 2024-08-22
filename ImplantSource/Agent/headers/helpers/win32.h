//
// Created by Brendan Ortiz on 8/12/2024.
//

#ifndef WIN32_H
#define WIN32_H

#include "../../headers/agent/agent.h"


typedef struct _PIPE {

    HANDLE hReadPipe;
    HANDLE hWritePipe;

}PIPE, *PPIPE;

HANDLE GetProcessHandleFromPID(DWORD dwPID);
HANDLE GetProcessHandleFromStr(LPCSTR ProcessName);
BOOL NtProcessCreate(IN PWSTR TargetProcess, IN PWSTR ProcessParameters, IN PWSTR ProcessPath, IN HANDLE hParentProcess, IN BOOL PIPED, OUT PHANDLE hProcess, OUT PHANDLE hThread);
BOOL ProcessCreate(IN BOOL PIPED, IN LPSTR ApplicationName, IN LPSTR CommandLine, OUT LPPROCESS_INFORMATION pi, OUT PPIPE *pPipe);
BOOL PipeCreate(OUT PPIPE pPipe);
INT PipeRead(IN PPIPE pPipe, PVOID* Buffer, PDWORD dwRead);

#endif //WIN32_H
