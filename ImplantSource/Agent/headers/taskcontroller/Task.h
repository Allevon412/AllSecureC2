//
// Created by Brendan Ortiz on 8/21/2024.
//

#ifndef TASK_H
#define TASK_H

#include "../agent/agent.h"


#define TASK_TYPE_THREAD    0x01
#define TASK_TYPE_PROCESS   0x02
#define TASK_TRACK_PROCESS  0x03

#define TASK_STATUS_RUNNING 0x01
#define TASK_STATUS_STOPPED 0x02
#define TASK_STATUS_PAUSED  0x03

typedef struct _TASK {

    DWORD   dwRequestID;
    DWORD   dwTaskID;
    SHORT   shTaskType;
    SHORT   shTaskStatus;
    PVOID   pTaskData;
    HANDLE  hTaskHandle;
    struct _TASK* pNext;

} TASK, *PTASK;

extern UINT TotalTaskCount;

VOID TaskAdd(DWORD dwRequestID, DWORD dwTaskID, SHORT shTaskType, SHORT shTaskStatus, PVOID pTaskData, HANDLE hTaskHandle);
VOID CheckTasks();
VOID TaskRemove(DWORD dwTaskID);

#endif //TASK_H
