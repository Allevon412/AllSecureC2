#pragma once
#include <WinSock2.h>

#include <winternl.h> //we can remove this if we resolve all functions in NT dynamically.
#include <Windows.h>


#include <winhttp.h>
#include <iphlpapi.h>

#define AES_BLOCKLEN 16
#define NTSUCCESS 0
#define UNLEN 256

//HTTP APIS
//WINHTTP APIS
typedef HINTERNET(WINAPI* t_WinHttpOpen)(
	LPCWSTR pszAgentW,
	DWORD   dwAccessType,
	LPCWSTR pszProxyW,
	LPCWSTR pszProxyBypassW,
	DWORD   dwFlags
	);

typedef HINTERNET(WINAPI* t_WinHttpConnect)(
	HINTERNET     hSession,
	LPCWSTR       pswzServerName,
	INTERNET_PORT nServerPort,
	DWORD         dwReserved
	);



typedef HINTERNET(WINAPI* t_WinHttpOpenRequest)(
	HINTERNET hConnect,
	LPCWSTR   pwszVerb,
	LPCWSTR   pwszObjectName,
	LPCWSTR   pwszVersion,
	LPCWSTR   pwszReferrer,
	LPCWSTR* ppwszAcceptTypes,
	DWORD     dwFlags
	);

typedef BOOL(WINAPI* t_WinHttpSetOption)(
	HINTERNET hInternet,
	DWORD     dwOption,
	LPVOID    lpBuffer,
	DWORD     dwBufferLength
	);

typedef BOOL(WINAPI* t_WinHttpAddRequestHeaders)(
	HINTERNET hRequest,
	LPCWSTR   pwszHeaders,
	DWORD     dwHeadersLength,
	DWORD     dwModifiers
	);


typedef BOOL(WINAPI* t_WinHttpSendRequest)(
HINTERNET hRequest,
	LPCWSTR   pwszHeaders,
	DWORD     dwHeadersLength,
	LPVOID    lpOptional,
	DWORD     dwOptionalLength,
	DWORD     dwTotalLength,
	DWORD_PTR  dwContext
	);

typedef BOOL(WINAPI* t_WinHttpReceiveResponse)(
	HINTERNET hRequest,
	LPVOID    lpReserved
	);

typedef BOOL(WINAPI* t_WinHttpQueryHeaders)(
	HINTERNET hRequest,
	DWORD     dwInfoLevel,
	LPCWSTR   pwszName,
	LPVOID    lpBuffer,
	LPDWORD   lpdwBufferLength,
	LPDWORD   lpdwIndex
	);

typedef BOOL(WINAPI* t_WinHttpReadData)(
	HINTERNET hRequest,
	LPVOID    lpBuffer,
	DWORD     dwNumberOfBytesToRead,
	LPDWORD   lpdwNumberOfBytesRead
	);
typedef BOOL(WINAPI* t_WinHttpCloseHandle)(
	HINTERNET hInternet
	);



// enumeration APIS
//NTDLL APIS
typedef NTSTATUS(NTAPI* t_RtlGetVersion)(
	PRTL_OSVERSIONINFOW lpVersionInformation
	);

typedef NTSTATUS(NTAPI* t_NtClose)(
	HANDLE Handle
	);
#define NtCurrentThread() ((HANDLE)(LONG_PTR)-2)
#define NtCurrentProcess() ((HANDLE)(LONG_PTR)-1)
typedef NTSTATUS(NTAPI* t_NtOpenProcessToken)(
	HANDLE ProcessHandle,
	ACCESS_MASK DesiredAccess,
	PHANDLE TokenHandle
);

typedef NTSTATUS(NTAPI* t_NtOpenThreadToken)(
	HANDLE ThreadHandle,
	ACCESS_MASK DesiredAccess,
	BOOLEAN OpenAsSelf,
	PHANDLE TokenHandle
);

typedef NTSTATUS(NTAPI* t_NtQueryInformationToken)(
	HANDLE TokenHandle,
	TOKEN_INFORMATION_CLASS TokenInformationClass,
	PVOID TokenInformation,
	ULONG TokenInformationLength,
	PULONG ReturnLength
);

typedef PVOID(NTAPI* t_RtlAllocateHeap) (
	PVOID  HeapHandle,
	ULONG  Flags,
	SIZE_T Size
);
typedef NTSTATUS(NTAPI* t_RtlReAllocateHeap) (
	IN PVOID                HeapHandle,
	IN ULONG                Flags,
	IN PVOID                MemoryPointer,
	IN ULONG                Size
);

typedef NTSTATUS(NTAPI* t_RtlRandomEx)(
	PULONG Seed
);

typedef NTSTATUS(NTAPI* t_NtGetTickCount)(
	);


//IPhlpapi APIS
typedef DWORD(WINAPI* t_GetAdaptersInfo)(
	PIP_ADAPTER_INFO pAdapterInfo,
	PULONG           pOutBufLen
	);


//ADVAPI APIS
typedef BOOL(WINAPI* t_GetUserNameA)(
	LPSTR   lpBuffer,
	LPDWORD pcbBuffer
	);



//KERNEL32 APIS
typedef BOOL(WINAPI* t_GetComputerNameExA)(
	COMPUTER_NAME_FORMAT NameType,
	LPSTR                lpBuffer,
	LPDWORD              nSize
	);

typedef LPVOID(WINAPI* t_LocalAlloc)(
	UINT   uFlags,
	SIZE_T uBytes
	);
typedef LPVOID(WINAPI* t_LocalReAlloc)(
	LPVOID lpMem,
	SIZE_T uBytes,
	UINT   uFlags
	);
typedef BOOL(WINAPI* t_LocalFree)(
	HLOCAL hMem
	);
typedef BOOL(WINAPI* t_GetLocalTime)(
	LPSYSTEMTIME lpSystemTime
	);

typedef VOID(WINAPI* t_GetSystemTimeAsFileTime)(
	LPFILETIME lpSystemTimeAsFileTime
	);

//USER32 APIS
typedef int (WINAPI* t_GetSystemMetrics)(
	int nIndex
	);


/*--------------------------------------------------------------------
  STRUCTURES
--------------------------------------------------------------------*/

typedef struct _INT_LSA_UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} _INT_LSA_UNICODE_STRING, * P_INT_LSA_UNICODE_STRING, _INT_UNICODE_STRING, * P_INT_UNICODE_STRING, * P_INT_UNICODE_STR;

typedef struct _INT_LDR_MODULE {
	LIST_ENTRY              InLoadOrderModuleList;
	LIST_ENTRY              InMemoryOrderModuleList;
	LIST_ENTRY              InInitializationOrderModuleList;
	PVOID                   BaseAddress;
	PVOID                   EntryPoint;
	ULONG                   SizeOfImage;
	_INT_UNICODE_STRING          FullDllName;
	_INT_UNICODE_STRING          BaseDllName;
	ULONG                   Flags;
	SHORT                   LoadCount;
	SHORT                   TlsIndex;
	LIST_ENTRY              HashTableEntry;
	ULONG                   TimeDateStamp;
} _INT_LDR_MODULE, * P_INT_LDR_MODULE;

typedef struct _INT_PEB_LDR_DATA {
	ULONG                   Length;
	ULONG                   Initialized;
	PVOID                   SsHandle;
	LIST_ENTRY              InLoadOrderModuleList;
	LIST_ENTRY              InMemoryOrderModuleList;
	LIST_ENTRY              InInitializationOrderModuleList;
} _INT_PEB_LDR_DATA, * P_INT_PEB_LDR_DATA;

struct _INT_STRING64
{
	USHORT Length;                                                          //0x0
	USHORT MaximumLength;                                                   //0x2
	ULONGLONG Buffer;                                                       //0x8
};

#ifdef _WIN64
//0x7c8 bytes (sizeof)
typedef struct _PEB64
{
	UCHAR InheritedAddressSpace;                                            //0x0
	UCHAR ReadImageFileExecOptions;                                         //0x1
	UCHAR BeingDebugged;                                                    //0x2
	union
	{
		UCHAR BitField;                                                     //0x3
		struct
		{
			UCHAR ImageUsesLargePages : 1;                                    //0x3
			UCHAR IsProtectedProcess : 1;                                     //0x3
			UCHAR IsImageDynamicallyRelocated : 1;                            //0x3
			UCHAR SkipPatchingUser32Forwarders : 1;                           //0x3
			UCHAR IsPackagedProcess : 1;                                      //0x3
			UCHAR IsAppContainer : 1;                                         //0x3
			UCHAR IsProtectedProcessLight : 1;                                //0x3
			UCHAR IsLongPathAwareProcess : 1;                                 //0x3
		};
	};
	UCHAR Padding0[4];                                                      //0x4
	ULONGLONG Mutant;                                                       //0x8
	ULONGLONG ImageBaseAddress;                                             //0x10
	ULONGLONG Ldr;                                                          //0x18
	ULONGLONG ProcessParameters;                                            //0x20
	ULONGLONG SubSystemData;                                                //0x28
	ULONGLONG ProcessHeap;                                                  //0x30
	ULONGLONG FastPebLock;                                                  //0x38
	ULONGLONG AtlThunkSListPtr;                                             //0x40
	ULONGLONG IFEOKey;                                                      //0x48
	union
	{
		ULONG CrossProcessFlags;                                            //0x50
		struct
		{
			ULONG ProcessInJob : 1;                                           //0x50
			ULONG ProcessInitializing : 1;                                    //0x50
			ULONG ProcessUsingVEH : 1;                                        //0x50
			ULONG ProcessUsingVCH : 1;                                        //0x50
			ULONG ProcessUsingFTH : 1;                                        //0x50
			ULONG ProcessPreviouslyThrottled : 1;                             //0x50
			ULONG ProcessCurrentlyThrottled : 1;                              //0x50
			ULONG ProcessImagesHotPatched : 1;                                //0x50
			ULONG ReservedBits0 : 24;                                         //0x50
		};
	};
	UCHAR Padding1[4];                                                      //0x54
	union
	{
		ULONGLONG KernelCallbackTable;                                      //0x58
		ULONGLONG UserSharedInfoPtr;                                        //0x58
	};
	ULONG SystemReserved;                                                   //0x60
	ULONG AtlThunkSListPtr32;                                               //0x64
	ULONGLONG ApiSetMap;                                                    //0x68
	ULONG TlsExpansionCounter;                                              //0x70
	UCHAR Padding2[4];                                                      //0x74
	ULONGLONG TlsBitmap;                                                    //0x78
	ULONG TlsBitmapBits[2];                                                 //0x80
	ULONGLONG ReadOnlySharedMemoryBase;                                     //0x88
	ULONGLONG SharedData;                                                   //0x90
	ULONGLONG ReadOnlyStaticServerData;                                     //0x98
	ULONGLONG AnsiCodePageData;                                             //0xa0
	ULONGLONG OemCodePageData;                                              //0xa8
	ULONGLONG UnicodeCaseTableData;                                         //0xb0
	ULONG NumberOfProcessors;                                               //0xb8
	ULONG NtGlobalFlag;                                                     //0xbc
	union _LARGE_INTEGER CriticalSectionTimeout;                            //0xc0
	ULONGLONG HeapSegmentReserve;                                           //0xc8
	ULONGLONG HeapSegmentCommit;                                            //0xd0
	ULONGLONG HeapDeCommitTotalFreeThreshold;                               //0xd8
	ULONGLONG HeapDeCommitFreeBlockThreshold;                               //0xe0
	ULONG NumberOfHeaps;                                                    //0xe8
	ULONG MaximumNumberOfHeaps;                                             //0xec
	ULONGLONG ProcessHeaps;                                                 //0xf0
	ULONGLONG GdiSharedHandleTable;                                         //0xf8
	ULONGLONG ProcessStarterHelper;                                         //0x100
	ULONG GdiDCAttributeList;                                               //0x108
	UCHAR Padding3[4];                                                      //0x10c
	ULONGLONG LoaderLock;                                                   //0x110
	ULONG OSMajorVersion;                                                   //0x118
	ULONG OSMinorVersion;                                                   //0x11c
	USHORT OSBuildNumber;                                                   //0x120
	USHORT OSCSDVersion;                                                    //0x122
	ULONG OSPlatformId;                                                     //0x124
	ULONG ImageSubsystem;                                                   //0x128
	ULONG ImageSubsystemMajorVersion;                                       //0x12c
	ULONG ImageSubsystemMinorVersion;                                       //0x130
	UCHAR Padding4[4];                                                      //0x134
	ULONGLONG ActiveProcessAffinityMask;                                    //0x138
	ULONG GdiHandleBuffer[60];                                              //0x140
	ULONGLONG PostProcessInitRoutine;                                       //0x230
	ULONGLONG TlsExpansionBitmap;                                           //0x238
	ULONG TlsExpansionBitmapBits[32];                                       //0x240
	ULONG SessionId;                                                        //0x2c0
	UCHAR Padding5[4];                                                      //0x2c4
	union _ULARGE_INTEGER AppCompatFlags;                                   //0x2c8
	union _ULARGE_INTEGER AppCompatFlagsUser;                               //0x2d0
	ULONGLONG pShimData;                                                    //0x2d8
	ULONGLONG AppCompatInfo;                                                //0x2e0
	struct _INT_STRING64 CSDVersion;                                            //0x2e8
	ULONGLONG ActivationContextData;                                        //0x2f8
	ULONGLONG ProcessAssemblyStorageMap;                                    //0x300
	ULONGLONG SystemDefaultActivationContextData;                           //0x308
	ULONGLONG SystemAssemblyStorageMap;                                     //0x310
	ULONGLONG MinimumStackCommit;                                           //0x318
	ULONGLONG SparePointers[4];                                             //0x320
	ULONG SpareUlongs[5];                                                   //0x340
	ULONGLONG WerRegistrationData;                                          //0x358
	ULONGLONG WerShipAssertPtr;                                             //0x360
	ULONGLONG pUnused;                                                      //0x368
	ULONGLONG pImageHeaderHash;                                             //0x370
	union
	{
		ULONG TracingFlags;                                                 //0x378
		struct
		{
			ULONG HeapTracingEnabled : 1;                                     //0x378
			ULONG CritSecTracingEnabled : 1;                                  //0x378
			ULONG LibLoaderTracingEnabled : 1;                                //0x378
			ULONG SpareTracingBits : 29;                                      //0x378
		};
	};
	UCHAR Padding6[4];                                                      //0x37c
	ULONGLONG CsrServerReadOnlySharedMemoryBase;                            //0x380
	ULONGLONG TppWorkerpListLock;                                           //0x388
	struct LIST_ENTRY64 TppWorkerpList;                                     //0x390
	ULONGLONG WaitOnAddressHashTable[128];                                  //0x3a0
	ULONGLONG TelemetryCoverageHeader;                                      //0x7a0
	ULONG CloudFileFlags;                                                   //0x7a8
	ULONG CloudFileDiagFlags;                                               //0x7ac
	CHAR PlaceholderCompatibilityMode;                                      //0x7b0
	CHAR PlaceholderCompatibilityModeReserved[7];                           //0x7b1
	ULONGLONG LeapSecondData;                                               //0x7b8
	union
	{
		ULONG LeapSecondFlags;                                              //0x7c0
		struct
		{
			ULONG SixtySecondEnabled : 1;                                     //0x7c0
			ULONG Reserved : 31;                                              //0x7c0
		};
	};
	ULONG NtGlobalFlag2;                                                    //0x7c4
} _INT_PEB, *P_INT_PEB;
#else
//0x480 bytes (sizeof)
typedef struct _PEB32
{
	UCHAR InheritedAddressSpace;                                            //0x0
	UCHAR ReadImageFileExecOptions;                                         //0x1
	UCHAR BeingDebugged;                                                    //0x2
	union
	{
		UCHAR BitField;                                                     //0x3
		struct
		{
			UCHAR ImageUsesLargePages : 1;                                    //0x3
			UCHAR IsProtectedProcess : 1;                                     //0x3
			UCHAR IsImageDynamicallyRelocated : 1;                            //0x3
			UCHAR SkipPatchingUser32Forwarders : 1;                           //0x3
			UCHAR IsPackagedProcess : 1;                                      //0x3
			UCHAR IsAppContainer : 1;                                         //0x3
			UCHAR IsProtectedProcessLight : 1;                                //0x3
			UCHAR IsLongPathAwareProcess : 1;                                 //0x3
		};
	};
	ULONG Mutant;                                                           //0x4
	ULONG ImageBaseAddress;                                                 //0x8
	ULONG Ldr;                                                              //0xc
	ULONG ProcessParameters;                                                //0x10
	ULONG SubSystemData;                                                    //0x14
	ULONG ProcessHeap;                                                      //0x18
	ULONG FastPebLock;                                                      //0x1c
	ULONG AtlThunkSListPtr;                                                 //0x20
	ULONG IFEOKey;                                                          //0x24
	union
	{
		ULONG CrossProcessFlags;                                            //0x28
		struct
		{
			ULONG ProcessInJob : 1;                                           //0x28
			ULONG ProcessInitializing : 1;                                    //0x28
			ULONG ProcessUsingVEH : 1;                                        //0x28
			ULONG ProcessUsingVCH : 1;                                        //0x28
			ULONG ProcessUsingFTH : 1;                                        //0x28
			ULONG ProcessPreviouslyThrottled : 1;                             //0x28
			ULONG ProcessCurrentlyThrottled : 1;                              //0x28
			ULONG ProcessImagesHotPatched : 1;                                //0x28
			ULONG ReservedBits0 : 24;                                         //0x28
		};
	};
	union
	{
		ULONG KernelCallbackTable;                                          //0x2c
		ULONG UserSharedInfoPtr;                                            //0x2c
	};
	ULONG SystemReserved;                                                   //0x30
	ULONG AtlThunkSListPtr32;                                               //0x34
	ULONG ApiSetMap;                                                        //0x38
	ULONG TlsExpansionCounter;                                              //0x3c
	ULONG TlsBitmap;                                                        //0x40
	ULONG TlsBitmapBits[2];                                                 //0x44
	ULONG ReadOnlySharedMemoryBase;                                         //0x4c
	ULONG SharedData;                                                       //0x50
	ULONG ReadOnlyStaticServerData;                                         //0x54
	ULONG AnsiCodePageData;                                                 //0x58
	ULONG OemCodePageData;                                                  //0x5c
	ULONG UnicodeCaseTableData;                                             //0x60
	ULONG NumberOfProcessors;                                               //0x64
	ULONG NtGlobalFlag;                                                     //0x68
	union _LARGE_INTEGER CriticalSectionTimeout;                            //0x70
	ULONG HeapSegmentReserve;                                               //0x78
	ULONG HeapSegmentCommit;                                                //0x7c
	ULONG HeapDeCommitTotalFreeThreshold;                                   //0x80
	ULONG HeapDeCommitFreeBlockThreshold;                                   //0x84
	ULONG NumberOfHeaps;                                                    //0x88
	ULONG MaximumNumberOfHeaps;                                             //0x8c
	ULONG ProcessHeaps;                                                     //0x90
	ULONG GdiSharedHandleTable;                                             //0x94
	ULONG ProcessStarterHelper;                                             //0x98
	ULONG GdiDCAttributeList;                                               //0x9c
	ULONG LoaderLock;                                                       //0xa0
	ULONG OSMajorVersion;                                                   //0xa4
	ULONG OSMinorVersion;                                                   //0xa8
	USHORT OSBuildNumber;                                                   //0xac
	USHORT OSCSDVersion;                                                    //0xae
	ULONG OSPlatformId;                                                     //0xb0
	ULONG ImageSubsystem;                                                   //0xb4
	ULONG ImageSubsystemMajorVersion;                                       //0xb8
	ULONG ImageSubsystemMinorVersion;                                       //0xbc
	ULONG ActiveProcessAffinityMask;                                        //0xc0
	ULONG GdiHandleBuffer[34];                                              //0xc4
	ULONG PostProcessInitRoutine;                                           //0x14c
	ULONG TlsExpansionBitmap;                                               //0x150
	ULONG TlsExpansionBitmapBits[32];                                       //0x154
	ULONG SessionId;                                                        //0x1d4
	union _ULARGE_INTEGER AppCompatFlags;                                   //0x1d8
	union _ULARGE_INTEGER AppCompatFlagsUser;                               //0x1e0
	ULONG pShimData;                                                        //0x1e8
	ULONG AppCompatInfo;                                                    //0x1ec
	struct _STRING32 CSDVersion;                                            //0x1f0
	ULONG ActivationContextData;                                            //0x1f8
	ULONG ProcessAssemblyStorageMap;                                        //0x1fc
	ULONG SystemDefaultActivationContextData;                               //0x200
	ULONG SystemAssemblyStorageMap;                                         //0x204
	ULONG MinimumStackCommit;                                               //0x208
	ULONG SparePointers[4];                                                 //0x20c
	ULONG SpareUlongs[5];                                                   //0x21c
	ULONG WerRegistrationData;                                              //0x230
	ULONG WerShipAssertPtr;                                                 //0x234
	ULONG pUnused;                                                          //0x238
	ULONG pImageHeaderHash;                                                 //0x23c
	union
	{
		ULONG TracingFlags;                                                 //0x240
		struct
		{
			ULONG HeapTracingEnabled : 1;                                     //0x240
			ULONG CritSecTracingEnabled : 1;                                  //0x240
			ULONG LibLoaderTracingEnabled : 1;                                //0x240
			ULONG SpareTracingBits : 29;                                      //0x240
		};
	};
	ULONGLONG CsrServerReadOnlySharedMemoryBase;                            //0x248
	ULONG TppWorkerpListLock;                                               //0x250
	struct LIST_ENTRY32 TppWorkerpList;                                     //0x254
	ULONG WaitOnAddressHashTable[128];                                      //0x25c
	ULONG TelemetryCoverageHeader;                                          //0x45c
	ULONG CloudFileFlags;                                                   //0x460
	ULONG CloudFileDiagFlags;                                               //0x464
	CHAR PlaceholderCompatibilityMode;                                      //0x468
	CHAR PlaceholderCompatibilityModeReserved[7];                           //0x469
	ULONG LeapSecondData;                                                   //0x470
	union
	{
		ULONG LeapSecondFlags;                                              //0x474
		struct
		{
			ULONG SixtySecondEnabled : 1;                                     //0x474
			ULONG Reserved : 31;                                              //0x474
		};
	};
	ULONG NtGlobalFlag2;                                                    //0x478
} PEB, *PPEB;
#endif

typedef struct _INT_CLIENT_ID {
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} _INT_CLIENT_ID, * P_INT_CLIENT_ID;

typedef struct _INT_TEB_ACTIVE_FRAME_CONTEXT {
	ULONG Flags;
	PCHAR FrameName;
} _INT_TEB_ACTIVE_FRAME_CONTEXT, * P_INT_TEB_ACTIVE_FRAME_CONTEXT;

typedef struct _INT_TEB_ACTIVE_FRAME {
	ULONG Flags;
	struct _TEB_ACTIVE_FRAME* Previous;
	P_INT_TEB_ACTIVE_FRAME_CONTEXT Context;
} _INT_TEB_ACTIVE_FRAME, * P_INT_TEB_ACTIVE_FRAME;

typedef struct _INT_GDI_TEB_BATCH {
	ULONG Offset;
	ULONG HDC;
	ULONG Buffer[310];
} _INT_GDI_TEB_BATCH, * P_INT_GDI_TEB_BATCH;

typedef PVOID PACTIVATION_CONTEXT;

typedef struct _INT_RTL_ACTIVATION_CONTEXT_STACK_FRAME {
	struct __RTL_ACTIVATION_CONTEXT_STACK_FRAME* Previous;
	PACTIVATION_CONTEXT ActivationContext;
	ULONG Flags;
} _INT_RTL_ACTIVATION_CONTEXT_STACK_FRAME, * P_INT_RTL_ACTIVATION_CONTEXT_STACK_FRAME;

typedef struct _INT_ACTIVATION_CONTEXT_STACK {
	P_INT_RTL_ACTIVATION_CONTEXT_STACK_FRAME ActiveFrame;
	LIST_ENTRY FrameListCache;
	ULONG Flags;
	ULONG NextCookieSequenceNumber;
	ULONG StackId;
} _INT_ACTIVATION_CONTEXT_STACK, * P_INT_ACTIVATION_CONTEXT_STACK;

//0x1838 bytes (sizeof)
typedef struct _INT_TEB
{
	struct _NT_TIB NtTib;                                                   //0x0
	VOID* EnvironmentPointer;                                               //0x38
	struct _INT_CLIENT_ID ClientId;                                             //0x40
	VOID* ActiveRpcHandle;                                                  //0x50
	VOID* ThreadLocalStoragePointer;                                        //0x58
#ifndef _WIN64
	struct PPEB ProcessEnvironmentBlock;                                   //0x60
#else
	P_INT_PEB ProcessEnvironmentBlock;
#endif
	ULONG LastErrorValue;                                                   //0x68
	ULONG CountOfOwnedCriticalSections;                                     //0x6c
	VOID* CsrClientThread;                                                  //0x70
	VOID* Win32ThreadInfo;                                                  //0x78
	ULONG User32Reserved[26];                                               //0x80
	ULONG UserReserved[5];                                                  //0xe8
	VOID* WOW32Reserved;                                                    //0x100
	ULONG CurrentLocale;                                                    //0x108
	ULONG FpSoftwareStatusRegister;                                         //0x10c
	VOID* ReservedForDebuggerInstrumentation[16];                           //0x110
	VOID* SystemReserved1[30];                                              //0x190
	CHAR PlaceholderCompatibilityMode;                                      //0x280
	UCHAR PlaceholderHydrationAlwaysExplicit;                               //0x281
	CHAR PlaceholderReserved[10];                                           //0x282
	ULONG ProxiedProcessId;                                                 //0x28c
	struct _INT_ACTIVATION_CONTEXT_STACK _ActivationStack;                      //0x290
	UCHAR WorkingOnBehalfTicket[8];                                         //0x2b8
	LONG ExceptionCode;                                                     //0x2c0
	UCHAR Padding0[4];                                                      //0x2c4
	struct _INT_ACTIVATION_CONTEXT_STACK* ActivationContextStackPointer;        //0x2c8
	ULONGLONG InstrumentationCallbackSp;                                    //0x2d0
	ULONGLONG InstrumentationCallbackPreviousPc;                            //0x2d8
	ULONGLONG InstrumentationCallbackPreviousSp;                            //0x2e0
	ULONG TxFsContext;                                                      //0x2e8
	UCHAR InstrumentationCallbackDisabled;                                  //0x2ec
	UCHAR UnalignedLoadStoreExceptions;                                     //0x2ed
	UCHAR Padding1[2];                                                      //0x2ee
	struct _INT_GDI_TEB_BATCH GdiTebBatch;                                      //0x2f0
	struct _INT_CLIENT_ID RealClientId;                                         //0x7d8
	VOID* GdiCachedProcessHandle;                                           //0x7e8
	ULONG GdiClientPID;                                                     //0x7f0
	ULONG GdiClientTID;                                                     //0x7f4
	VOID* GdiThreadLocalInfo;                                               //0x7f8
	ULONGLONG Win32ClientInfo[62];                                          //0x800
	VOID* glDispatchTable[233];                                             //0x9f0
	ULONGLONG glReserved1[29];                                              //0x1138
	VOID* glReserved2;                                                      //0x1220
	VOID* glSectionInfo;                                                    //0x1228
	VOID* glSection;                                                        //0x1230
	VOID* glTable;                                                          //0x1238
	VOID* glCurrentRC;                                                      //0x1240
	VOID* glContext;                                                        //0x1248
	ULONG LastStatusValue;                                                  //0x1250
	UCHAR Padding2[4];                                                      //0x1254
	struct _INT_LSA_UNICODE_STRING StaticUnicodeString;                             //0x1258
	WCHAR StaticUnicodeBuffer[261];                                         //0x1268
	UCHAR Padding3[6];                                                      //0x1472
	VOID* DeallocationStack;                                                //0x1478
	VOID* TlsSlots[64];                                                     //0x1480
	struct _LIST_ENTRY TlsLinks;                                            //0x1680
	VOID* Vdm;                                                              //0x1690
	VOID* ReservedForNtRpc;                                                 //0x1698
	VOID* DbgSsReserved[2];                                                 //0x16a0
	ULONG HardErrorMode;                                                    //0x16b0
	UCHAR Padding4[4];                                                      //0x16b4
	VOID* Instrumentation[11];                                              //0x16b8
	struct _GUID ActivityId;                                                //0x1710
	VOID* SubProcessTag;                                                    //0x1720
	VOID* PerflibData;                                                      //0x1728
	VOID* EtwTraceData;                                                     //0x1730
	VOID* WinSockData;                                                      //0x1738
	ULONG GdiBatchCount;                                                    //0x1740
	union
	{
		struct _PROCESSOR_NUMBER CurrentIdealProcessor;                     //0x1744
		ULONG IdealProcessorValue;                                          //0x1744
		struct
		{
			UCHAR ReservedPad0;                                             //0x1744
			UCHAR ReservedPad1;                                             //0x1745
			UCHAR ReservedPad2;                                             //0x1746
			UCHAR IdealProcessor;                                           //0x1747
		};
	};
	ULONG GuaranteedStackBytes;                                             //0x1748
	UCHAR Padding5[4];                                                      //0x174c
	VOID* ReservedForPerf;                                                  //0x1750
	VOID* ReservedForOle;                                                   //0x1758
	ULONG WaitingOnLoaderLock;                                              //0x1760
	UCHAR Padding6[4];                                                      //0x1764
	VOID* SavedPriorityState;                                               //0x1768
	ULONGLONG ReservedForCodeCoverage;                                      //0x1770
	VOID* ThreadPoolData;                                                   //0x1778
	VOID** TlsExpansionSlots;                                               //0x1780
	VOID* DeallocationBStore;                                               //0x1788
	VOID* BStoreLimit;                                                      //0x1790
	ULONG MuiGeneration;                                                    //0x1798
	ULONG IsImpersonating;                                                  //0x179c
	VOID* NlsCache;                                                         //0x17a0
	VOID* pShimData;                                                        //0x17a8
	ULONG HeapData;                                                         //0x17b0
	UCHAR Padding7[4];                                                      //0x17b4
	VOID* CurrentTransactionHandle;                                         //0x17b8
	struct _TEB_ACTIVE_FRAME* ActiveFrame;                                  //0x17c0
	VOID* FlsData;                                                          //0x17c8
	VOID* PreferredLanguages;                                               //0x17d0
	VOID* UserPrefLanguages;                                                //0x17d8
	VOID* MergedPrefLanguages;                                              //0x17e0
	ULONG MuiImpersonation;                                                 //0x17e8
	union
	{
		volatile USHORT CrossTebFlags;                                      //0x17ec
		USHORT SpareCrossTebBits : 16;                                        //0x17ec
	};
	union
	{
		USHORT SameTebFlags;                                                //0x17ee
		struct
		{
			USHORT SafeThunkCall : 1;                                         //0x17ee
			USHORT InDebugPrint : 1;                                          //0x17ee
			USHORT HasFiberData : 1;                                          //0x17ee
			USHORT SkipThreadAttach : 1;                                      //0x17ee
			USHORT WerInShipAssertCode : 1;                                   //0x17ee
			USHORT RanProcessInit : 1;                                        //0x17ee
			USHORT ClonedThread : 1;                                          //0x17ee
			USHORT SuppressDebugMsg : 1;                                      //0x17ee
			USHORT DisableUserStackWalk : 1;                                  //0x17ee
			USHORT RtlExceptionAttached : 1;                                  //0x17ee
			USHORT InitialThread : 1;                                         //0x17ee
			USHORT SessionAware : 1;                                          //0x17ee
			USHORT LoadOwner : 1;                                             //0x17ee
			USHORT LoaderWorker : 1;                                          //0x17ee
			USHORT SkipLoaderInit : 1;                                        //0x17ee
			USHORT SpareSameTebBits : 1;                                      //0x17ee
		};
	};
	VOID* TxnScopeEnterCallback;                                            //0x17f0
	VOID* TxnScopeExitCallback;                                             //0x17f8
	VOID* TxnScopeContext;                                                  //0x1800
	ULONG LockCount;                                                        //0x1808
	LONG WowTebOffset;                                                      //0x180c
	VOID* ResourceRetValue;                                                 //0x1810
	VOID* ReservedForWdf;                                                   //0x1818
	ULONGLONG ReservedForCrt;                                               //0x1820
	struct _GUID EffectiveContainerId;                                      //0x1828

} _INT_TEB, *P_INT_TEB;

typedef struct _INT_LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	_INT_UNICODE_STRING FullDllName;
	_INT_UNICODE_STRING BaseDllName;
	ULONG Flags;
	WORD LoadCount;
	WORD TlsIndex;
	union {
		LIST_ENTRY HashLinks;
		struct {
			PVOID SectionPointer;
			ULONG CheckSum;
		};
	};
	union {
		ULONG TimeDateStamp;
		PVOID LoadedImports;
	};
	PACTIVATION_CONTEXT EntryPointActivationContext;
	PVOID PatchInformation;
	LIST_ENTRY ForwarderLinks;
	LIST_ENTRY ServiceTagLinks;
	LIST_ENTRY StaticLinks;
} _INT_LDR_DATA_TABLE_ENTRY, * P_INT_LDR_DATA_TABLE_ENTRY;


typedef struct _INT_INITIAL_TEB {
	PVOID                StackBase;
	PVOID                StackLimit;
	PVOID                StackCommit;
	PVOID                StackCommitMax;
	PVOID                StackReserved;
} _INT_INITIAL_TEB, * P_INT_INITIAL_TEB;

typedef struct _DataBuffer {
	BYTE* Buffer;
	ULONG BufferLength;
} DataBuffer, * pDataBuffer;


#define NtProcessHeap(agent) agent->pTeb->ProcessEnvironmentBlock->ProcessHeap

#define MAX_HOST_FAILURES 10
enum HOST_ROTATION {
	HOST_ROTATION_ROUND_ROBIN = 0,
	HOST_ROTATION_RANDOM,
	HOST_ROTATION_FAIL_OVER
};
