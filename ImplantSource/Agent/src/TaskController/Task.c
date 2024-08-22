//
// Created by Brendan Ortiz on 8/21/2024.
//

#include "../../headers/taskcontroller/Task.h"
#include "../../headers/helpers/win32.h"
#include "../../headers/package/package.h"

VOID TaskAdd(DWORD dwRequestID, DWORD dwTaskID, SHORT shTaskType, SHORT shTaskStatus, PVOID pTaskData, HANDLE hTaskHandle) {


    PTASK TaskList = NULL;
    PTASK Task = NULL;

    Task = agent->apis->pLocalAlloc(LPTR, sizeof(TASK));
    if(Task == NULL) {
        return;
    }

    Task->dwRequestID = dwRequestID;
    Task->dwTaskID = dwTaskID;
    Task->shTaskType = shTaskType;
    Task->shTaskStatus = shTaskStatus;
    Task->pTaskData = pTaskData;
    Task->hTaskHandle = hTaskHandle;
    Task->pNext = NULL;


    if(agent->TaskList == NULL) {
        agent->TaskList = Task;
        return;
    }

    TaskList = agent->TaskList;
    while(TaskList->pNext != NULL) {
        TaskList = TaskList->pNext;
    }
    TaskList->pNext = Task;
    agent->TaskList->TaskCount++;
}

VOID CheckTasks() {

    PTASK TaskList = NULL;
    TaskList = agent->TaskList;
    pPackage pack = NULL;


    if(TaskList == NULL)
        return;

    while (TaskList != NULL) {

        switch(TaskList->shTaskType) {

            case TASK_TYPE_THREAD: {
                //DO SOMETHING HERE
                break;
            } // case TASK_TYPE_THREAD

            case TASK_TYPE_PROCESS: {

                if(TaskList->shTaskStatus == TASK_STATUS_RUNNING) {
                    DWORD Status = 0;
                    agent->apis->pGetExitCodeProcess(TaskList->hTaskHandle, &Status);
                    if (Status != STILL_ACTIVE)
                        TaskList->shTaskStatus = TASK_STATUS_STOPPED;

                }
                break;
            } // case TASK_TYPE_PROCESS


            case TASK_TRACK_PROCESS: {

                if(TaskList->shTaskStatus == TASK_STATUS_RUNNING) {
                    DWORD Status = 0;
                    agent->apis->pGetExitCodeProcess(TaskList->hTaskHandle, &Status);
                    if (Status != STILL_ACTIVE) {
                        TaskList->shTaskStatus = TASK_STATUS_STOPPED;

                        PVOID Buffer = NULL;
                        DWORD dwBytesRead = 0;

                        PipeRead((PPIPE) TaskList->pTaskData, Buffer, &dwBytesRead);
                        if(dwBytesRead) {
                            pack = CreateDataPackage(SEND_DATA, AGENT_OUTPUT);
                            AddBytesToPackage(pack, Buffer, dwBytesRead);

                            RtlSecureZeroMemory(Buffer, dwBytesRead);
                            agent->apis->pLocalFree(Buffer);

                            AddPackageToAgentPackageList(pack);
                        }

                        agent->apis->pNtClose(TaskList->hTaskHandle);
                        TaskList->hTaskHandle = NULL;

                        if( ((PPIPE) TaskList->pTaskData)->hReadPipe) {
                            agent->apis->pNtClose(((PPIPE) TaskList->pTaskData)->hReadPipe);
                            ((PPIPE) TaskList->pTaskData)->hReadPipe = NULL;
                        }
                        if(((PPIPE) TaskList->pTaskData)->hWritePipe) {
                            agent->apis->pNtClose(((PPIPE) TaskList->pTaskData)->hWritePipe);
                            ((PPIPE) TaskList->pTaskData)->hWritePipe = NULL;
                        }

                        RtlSecureZeroMemory(TaskList->pTaskData, sizeof(PIPE));
                        agent->apis->pLocalFree(TaskList->pTaskData);
                        TaskList->pTaskData = NULL;
                        TaskRemove(TaskList->dwTaskID);
                    }
                }

                break;
            }
            default:
                break;

        } //switch

    }
}

VOID TaskRemove(DWORD dwTaskID) {

    PTASK TaskList = NULL;
    PTASK TaskToRemove = NULL;

    TaskList = agent->TaskList;

    if(TaskList == NULL)
        return;

    if(TaskList->dwTaskID == dwTaskID) {
        TaskToRemove = TaskList;
        agent->TaskList = TaskList->pNext;
        goto _END;
    }

    do {

        if(TaskList->pNext->dwTaskID == dwTaskID) {
            TaskToRemove = TaskList->pNext;
            TaskList->pNext = TaskList->pNext->pNext;
            break;
        }
        TaskList = TaskList->pNext;

    }while(TaskList->pNext != NULL);

    _END:
    if(!TaskToRemove)
        return;

    if(TaskToRemove->hTaskHandle)
        agent->apis->pNtClose(TaskToRemove->hTaskHandle);

    if(TaskToRemove->pTaskData) {
        RtlSecureZeroMemory(TaskToRemove, sizeof(PIPE));
        agent->apis->pLocalFree(TaskToRemove->pTaskData);
        TaskToRemove->pTaskData = NULL;
    }

    RtlSecureZeroMemory(TaskToRemove, sizeof(TASK));
    agent->apis->pLocalFree(TaskToRemove);
    TaskToRemove = NULL;

}