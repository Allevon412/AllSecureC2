
#include "headers/agent/agent.h"

/*
    Main template for the agent. This is the entry point for the agent.
    All the agent's functionality is implemented in the agent.c file.
    This will be the standard for each version of the agent. I.E. Service, DLL, etc.
*/


int main()
{
    HANDLE ThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AgentMain, NULL, 0, NULL);
    WaitForSingleObject(ThreadHandle, INFINITE);
    //AgentMain();

    return 0;
}

