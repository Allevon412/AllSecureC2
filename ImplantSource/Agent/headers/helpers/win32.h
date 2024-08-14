//
// Created by Brendan Ortiz on 8/12/2024.
//

#ifndef WIN32_H
#define WIN32_H

#include "../../headers/agent/agent.h"

HANDLE GetProcessHandleFromPID(DWORD dwPID);
HANDLE GetProcessHandleFromStr(LPCSTR ProcessName);
BOOL ProcessCreate(IN PWSTR TargetProcess, IN PWSTR ProcessParameters, IN PWSTR ProcessPath, IN HANDLE hParentProcess, IN BOOL PIPED, OUT PHANDLE hProcess, OUT PHANDLE hThread);

#endif //WIN32_H
