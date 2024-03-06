#include <Windows.h>
#include <winternl.h>
#include <stdio.h>
#include "implant.h"


#define NTSUCCESS 0
#define UNLEN 256

/*
NTSYSAPI NTSTATUS RtlGetVersion(
	[out] PRTL_OSVERSIONINFOW lpVersionInformation
);
*/

typedef NTSTATUS(NTAPI* t_RtlGetVersion)(
	PRTL_OSVERSIONINFOW lpVersionInformation
	);
typedef BOOL(WINAPI* t_GetUserNameA)(
	LPSTR   lpBuffer,
	LPDWORD pcbBuffer
	);
typedef BOOL(WINAPI* t_GetComputerNameExA)(
	COMPUTER_NAME_FORMAT NameType,
	LPSTR                lpBuffer,
	LPDWORD              nSize
);

LPSTR GetOperatingSystemFunc();
LPSTR GetCompName();
LPSTR GetUser();
BOOL Enumerate(pAgent);
