#pragma once
#include "agent.h"



HANDLE PerformRequest(pAgent agent, BYTE* Buffer, SIZE_T BufferLength);
BOOL GetCommand();
