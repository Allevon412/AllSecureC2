#include <Windows.h>
#include <winternl.h>
#include <stdio.h>


#define NTSUCCESS 0
/*
NTSYSAPI NTSTATUS RtlGetVersion(
	[out] PRTL_OSVERSIONINFOW lpVersionInformation
);
*/

typedef NTSTATUS(WINAPI* t_RtlGetVersion)(
	PRTL_OSVERSIONINFOW lpVersionInformation
	);

RTL_OSVERSIONINFOEXW GetOperatingSystemFunc();