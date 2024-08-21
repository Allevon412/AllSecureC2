#pragma once
#include <WinSock2.h>

#include <winternl.h>
#include <Windows.h>

#include <winhttp.h>
#include <iphlpapi.h>

#include <tlhelp32.h>

#define AES_BLOCKLEN 16
#define NTSUCCESS 0
#define UNLEN 256

typedef struct _EXTENDED_PROCESS_INFORMATION {
	PROCESSINFOCLASS ExtendedProcessInfo;
	PVOID ExtendedProcessInfoBuffer;
} EXTENDED_PROCESS_INFORMATION, *PEXTENDED_PROCESS_INFORMATION;

typedef enum INT_EVENT_TYPE
{
	NotificationEvent,
	SynchronizationEvent
} INT_EVENT_TYPE;



typedef enum MEMORY_INFORMATION_CLASS {

	MemoryBasicInformation

} MEMORY_INFORMATION_CLASS;

typedef enum VIRTUAL_MEMORY_INFORMATION_CLASS {

	VmPrefetchInformation,
	VmPagePriorityInformation,
	VmCfgCallTargetInformation

} VIRTUAL_MEMORY_INFORMATION_CLASS;

typedef struct _MEMORY_RANGE_ENTRY
{
  	PVOID VirtualAddress;
  	SIZE_T NumberOfBytes;
} MEMORY_RANGE_ENTRY, *PMEMORY_RANGE_ENTRY;

typedef struct _INT_CLIENT_ID {
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} _INT_CLIENT_ID, * P_INT_CLIENT_ID;

/*--------------------------------------------------------------------
  STRUCTURES
--------------------------------------------------------------------*/


typedef enum _LDR_DLL_LOAD_REASON {
	LoadReasonStaticDependency,
	LoadReasonStaticForwarderDependency,
	LoadReasonDynamicForwarderDependency,
	LoadReasonDelayloadDependency,
	LoadReasonDynamicLoad,
	LoadReasonAsImageLoad,
	LoadReasonAsDataLoad,
	LoadReasonUnknown = -1
} LDR_DLL_LOAD_REASON;

//0x18 bytes (sizeof)
typedef struct _RTL_BALANCED_NODE
{
	union
	{
		struct _RTL_BALANCED_NODE* Children[2];                             //0x0
		struct
		{
			struct _RTL_BALANCED_NODE* Left;                                //0x0
			struct _RTL_BALANCED_NODE* Right;                               //0x8
		};
	};
	union
	{
		struct
		{
			UCHAR Red : 1;                                                    //0x10
			UCHAR Balance : 2;                                                //0x10
		};
		ULONGLONG ParentValue;                                              //0x10
	};
}_RTL_BALANCED_NODE, * P_RTL_BALANCED_NODE;

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

//0x120 bytes (sizeof)
typedef struct _INT_LDR_DATA_TABLE_ENTRY
{
	struct _LIST_ENTRY InLoadOrderLinks;                                    //0x0
	struct _LIST_ENTRY InMemoryOrderLinks;                                  //0x10
	struct _LIST_ENTRY InInitializationOrderLinks;                          //0x20
	VOID* DllBase;                                                          //0x30
	VOID* EntryPoint;                                                       //0x38
	ULONG SizeOfImage;                                                      //0x40
	struct _UNICODE_STRING FullDllName;                                     //0x48
	struct _UNICODE_STRING BaseDllName;                                     //0x58
	union
	{
		UCHAR FlagGroup[4];                                                 //0x68
		ULONG Flags;                                                        //0x68
		struct
		{
			ULONG PackagedBinary : 1;                                         //0x68
			ULONG MarkedForRemoval : 1;                                       //0x68
			ULONG ImageDll : 1;                                               //0x68
			ULONG LoadNotificationsSent : 1;                                  //0x68
			ULONG TelemetryEntryProcessed : 1;                                //0x68
			ULONG ProcessStaticImport : 1;                                    //0x68
			ULONG InLegacyLists : 1;                                          //0x68
			ULONG InIndexes : 1;                                              //0x68
			ULONG ShimDll : 1;                                                //0x68
			ULONG InExceptionTable : 1;                                       //0x68
			ULONG ReservedFlags1 : 2;                                         //0x68
			ULONG LoadInProgress : 1;                                         //0x68
			ULONG LoadConfigProcessed : 1;                                    //0x68
			ULONG EntryProcessed : 1;                                         //0x68
			ULONG ProtectDelayLoad : 1;                                       //0x68
			ULONG ReservedFlags3 : 2;                                         //0x68
			ULONG DontCallForThreads : 1;                                     //0x68
			ULONG ProcessAttachCalled : 1;                                    //0x68
			ULONG ProcessAttachFailed : 1;                                    //0x68
			ULONG CorDeferredValidate : 1;                                    //0x68
			ULONG CorImage : 1;                                               //0x68
			ULONG DontRelocate : 1;                                           //0x68
			ULONG CorILOnly : 1;                                              //0x68
			ULONG ChpeImage : 1;                                              //0x68
			ULONG ReservedFlags5 : 2;                                         //0x68
			ULONG Redirected : 1;                                             //0x68
			ULONG ReservedFlags6 : 2;                                         //0x68
			ULONG CompatDatabaseProcessed : 1;                                //0x68
		};
	};
	USHORT ObsoleteLoadCount;                                               //0x6c
	USHORT TlsIndex;                                                        //0x6e
	struct _LIST_ENTRY HashLinks;                                           //0x70
	ULONG TimeDateStamp;                                                    //0x80
	struct _ACTIVATION_CONTEXT* EntryPointActivationContext;                //0x88
	VOID* Lock;                                                             //0x90
	struct _LDR_DDAG_NODE* DdagNode;                                        //0x98
	struct _LIST_ENTRY NodeModuleLink;                                      //0xa0
	struct _LDRP_LOAD_CONTEXT* LoadContext;                                 //0xb0
	VOID* ParentDllBase;                                                    //0xb8
	VOID* SwitchBackContext;                                                //0xc0
	struct _RTL_BALANCED_NODE BaseAddressIndexNode;                         //0xc8
	struct _RTL_BALANCED_NODE MappingInfoIndexNode;                         //0xe0
	ULONGLONG OriginalBase;                                                 //0xf8
	union _LARGE_INTEGER LoadTime;                                          //0x100
	ULONG BaseNameHashValue;                                                //0x108
	enum _LDR_DLL_LOAD_REASON LoadReason;                                   //0x10c
	ULONG ImplicitPathOptions;                                              //0x110
	ULONG ReferenceCount;                                                   //0x114
	ULONG DependentLoadFlags;                                               //0x118
	UCHAR SigningLevel;                                                     //0x11c
}_INT_LDR_DATA_TABLE_ENTRY, * P_INT_LDR_DATA_TABLE_ENTRY;

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
	P_INT_LDR_DATA_TABLE_ENTRY Ldr;                                                          //0x18
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

typedef struct _DataBuffer {
	DWORD BufferLength;
	DWORD MaxBufferLength;
	BYTE* Buffer;
} DataBuffer, * pDataBuffer;

typedef struct _USTRING {
	DWORD Length;
	DWORD MaximumLength;
	PVOID Buffer;
}USTRING, *pUSTRING;

typedef struct _INT_INITIAL_TEB {
	PVOID                StackBase;
	PVOID                StackLimit;
	PVOID                StackCommit;
	PVOID                StackCommitMax;
	PVOID                StackReserved;
} _INT_INITIAL_TEB, * P_INT_INITIAL_TEB;

#define NtProcessHeap(agent) agent->pTeb->ProcessEnvironmentBlock->ProcessHeap

#define MAX_HOST_FAILURES 10
enum HOST_ROTATION {
	HOST_ROTATION_ROUND_ROBIN = 0,
	HOST_ROTATION_RANDOM,
	HOST_ROTATION_FAIL_OVER
};

#define STATUS_SUCCESS 0x00000000L



typedef struct _RTL_DRIVE_LETTER_CURDIR
{
	USHORT Flags;
	USHORT Length;
	ULONG TimeStamp;
	UNICODE_STRING DosPath;

} RTL_DRIVE_LETTER_CURDIR, * PRTL_DRIVE_LETTER_CURDIR;

typedef struct _CURDIR
{
	UNICODE_STRING DosPath;
	HANDLE Handle;

} CURDIR, * PCURDIR;


#define RTL_MAX_DRIVE_LETTERS 32

typedef struct _INT_RTL_USER_PROCESS_PARAMETERS
{
	ULONG MaximumLength; //0710
	ULONG Length; //0710

	ULONG Flags; //01
	ULONG DebugFlags; //00

	HANDLE ConsoleHandle; //4c
	ULONG ConsoleFlags; //00
	HANDLE StandardInput; //
	HANDLE StandardOutput; //b8
	HANDLE StandardError; //b8

	CURDIR CurrentDirectory;
	UNICODE_STRING DllPath;
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
	PWCHAR Environment;

	ULONG StartingX;
	ULONG StartingY;
	ULONG CountX;
	ULONG CountY;
	ULONG CountCharsX;
	ULONG CountCharsY;
	ULONG FillAttribute;

	ULONG WindowFlags;
	ULONG ShowWindowFlags;
	UNICODE_STRING WindowTitle;
	UNICODE_STRING DesktopInfo;
	UNICODE_STRING ShellInfo;
	UNICODE_STRING RuntimeData;
	RTL_DRIVE_LETTER_CURDIR CurrentDirectories[RTL_MAX_DRIVE_LETTERS];

	ULONG_PTR EnvironmentSize;
	ULONG_PTR EnvironmentVersion;
	PVOID PackageDependencyData;
	ULONG ProcessGroupId;
	ULONG LoaderThreads;

} INT_RTL_USER_PROCESS_PARAMETERS, * P_INT_RTL_USER_PROCESS_PARAMETERS;




typedef enum _PS_CREATE_STATE
{
	PsCreateInitialState,
	PsCreateFailOnFileOpen,
	PsCreateFailOnSectionCreate,
	PsCreateFailExeFormat,
	PsCreateFailMachineMismatch,
	PsCreateFailExeName,
	PsCreateSuccess,
	PsCreateMaximumStates

} PS_CREATE_STATE;

typedef struct _PS_CREATE_INFO
{
	SIZE_T Size;
	PS_CREATE_STATE State;
	union
	{
		struct
		{
			union
			{
				ULONG InitFlags;
				struct
				{
					UCHAR WriteOutputOnExit : 1;
					UCHAR DetectManifest : 1;
					UCHAR IFEOSkipDebugger : 1;
					UCHAR IFEODoNotPropagateKeyState : 1;
					UCHAR SpareBits1 : 4;
					UCHAR SpareBits2 : 8;
					USHORT ProhibitedImageCharacteristics : 16;
				} s1;
			} u1;
			ACCESS_MASK AdditionalFileAccess;
		} InitState;

		struct
		{
			HANDLE FileHandle;
		} FailSection;

		struct
		{
			USHORT DllCharacteristics;
		} ExeFormat;

		struct
		{
			HANDLE IFEOKey;
		} ExeName;

		struct
		{
			union
			{
				ULONG OutputFlags;
				struct
				{
					UCHAR ProtectedProcess : 1;
					UCHAR AddressSpaceOverride : 1;
					UCHAR DevOverrideEnabled : 1;
					UCHAR ManifestDetected : 1;
					UCHAR ProtectedProcessLight : 1;
					UCHAR SpareBits1 : 3;
					UCHAR SpareBits2 : 8;
					USHORT SpareBits3 : 16;
				} s2;
			} u2;
			HANDLE FileHandle;
			HANDLE SectionHandle;
			ULONGLONG UserProcessParametersNative;
			ULONG UserProcessParametersWow64;
			ULONG CurrentParameterFlags;
			ULONGLONG PebAddressNative;
			ULONG PebAddressWow64;
			ULONGLONG ManifestAddress;
			ULONG ManifestSize;
		} SuccessState;
	};

} PS_CREATE_INFO, * PPS_CREATE_INFO;


//\
https://github.com/winsiderss/systeminformer/blob/master/phnt/include/ntpsapi.h#L2047

typedef struct _PS_ATTRIBUTE
{
	ULONG_PTR Attribute;
	SIZE_T Size;
	union
	{
		ULONG_PTR Value;
		PVOID ValuePtr;
	};
	PSIZE_T ReturnLength;

} PS_ATTRIBUTE, * PPS_ATTRIBUTE;



typedef struct _PS_ATTRIBUTE_LIST
{
	SIZE_T TotalLength;
	PS_ATTRIBUTE Attributes[2];

} PS_ATTRIBUTE_LIST, * PPS_ATTRIBUTE_LIST;



#define PS_ATTRIBUTE_NUMBER_MASK    0x0000ffff
#define PS_ATTRIBUTE_THREAD         0x00010000 // Attribute may be used with thread creation
#define PS_ATTRIBUTE_INPUT          0x00020000 // Attribute is input only
#define PS_ATTRIBUTE_ADDITIVE       0x00040000 // Attribute may be "accumulated", e.g. bitmasks, counters, etc.

//\
https://github.com/winsiderss/systeminformer/blob/master/phnt/include/ntpsapi.h#L1930

typedef enum _PS_ATTRIBUTE_NUM
{
	PsAttributeParentProcess,                   // in HANDLE
	PsAttributeDebugPort,                       // in HANDLE
	PsAttributeToken,                           // in HANDLE
	PsAttributeClientId,                        // out PCLIENT_ID
	PsAttributeTebAddress,                      // out PTEB
	PsAttributeImageName,                       // in PWSTR
	PsAttributeImageInfo,                       // out PSECTION_IMAGE_INFORMATION
	PsAttributeMemoryReserve,                   // in PPS_MEMORY_RESERVE
	PsAttributePriorityClass,                   // in UCHAR
	PsAttributeErrorMode,                       // in ULONG
	PsAttributeStdHandleInfo,                   // in PPS_STD_HANDLE_INFO
	PsAttributeHandleList,                      // in PHANDLE
	PsAttributeGroupAffinity,                   // in PGROUP_AFFINITY
	PsAttributePreferredNode,                   // in PUSHORT
	PsAttributeIdealProcessor,                  // in PPROCESSOR_NUMBER
	PsAttributeUmsThread,                       // see MSDN UpdateProceThreadAttributeList (CreateProcessW) - in PUMS_CREATE_THREAD_ATTRIBUTES
	PsAttributeMitigationOptions,               // in UCHAR
	PsAttributeProtectionLevel,                 // in ULONG
	PsAttributeSecureProcess,                   // since THRESHOLD (Virtual Secure Mode, Device Guard)
	PsAttributeJobList,
	PsAttributeChildProcessPolicy,              // since THRESHOLD2
	PsAttributeAllApplicationPackagesPolicy,    // since REDSTONE
	PsAttributeWin32kFilter,
	PsAttributeSafeOpenPromptOriginClaim,
	PsAttributeBnoIsolation,
	PsAttributeDesktopAppPolicy,
	PsAttributeMax
} PS_ATTRIBUTE_NUM;

//\
https://github.com/winsiderss/systeminformer/blob/master/phnt/include/ntpsapi.h#L1974

#define PsAttributeValue(Number, Thread, Input, Additive)		\
    (((Number) & PS_ATTRIBUTE_NUMBER_MASK)	|					\
    ((Thread) ? PS_ATTRIBUTE_THREAD : 0)	|					\
    ((Input) ? PS_ATTRIBUTE_INPUT : 0)		|					\
    ((Additive) ? PS_ATTRIBUTE_ADDITIVE : 0))

// Specifies the parent process of the new process
#define PS_ATTRIBUTE_PARENT_PROCESS \
    PsAttributeValue(PsAttributeParentProcess, FALSE, TRUE, TRUE)
// Specifies the debug port to use
#define PS_ATTRIBUTE_DEBUG_PORT \
    PsAttributeValue(PsAttributeDebugPort, FALSE, TRUE, TRUE)
// Specifies the token to assign to the new process
#define PS_ATTRIBUTE_TOKEN \
    PsAttributeValue(PsAttributeToken, FALSE, TRUE, TRUE)
// Specifies the client ID to assign to the new process
#define PS_ATTRIBUTE_CLIENT_ID \
    PsAttributeValue(PsAttributeClientId, TRUE, FALSE, FALSE)
// Specifies the TEB address to use for the new process
#define PS_ATTRIBUTE_TEB_ADDRESS \
    PsAttributeValue(PsAttributeTebAddress, TRUE, FALSE, FALSE)
// Specifies the image name of the new process
#define PS_ATTRIBUTE_IMAGE_NAME \
    PsAttributeValue(PsAttributeImageName, FALSE, TRUE, FALSE)
// Specifies the image information of the new process
#define PS_ATTRIBUTE_IMAGE_INFO \
    PsAttributeValue(PsAttributeImageInfo, FALSE, FALSE, FALSE)
// Specifies the amount of memory to reserve for the new process
#define PS_ATTRIBUTE_MEMORY_RESERVE \
    PsAttributeValue(PsAttributeMemoryReserve, FALSE, TRUE, FALSE)
// Specifies the priority class to use for the new process
#define PS_ATTRIBUTE_PRIORITY_CLASS \
    PsAttributeValue(PsAttributePriorityClass, FALSE, TRUE, FALSE)
// Specifies the error mode to use for the new process
#define PS_ATTRIBUTE_ERROR_MODE \
    PsAttributeValue(PsAttributeErrorMode, FALSE, TRUE, FALSE)
// Specifies the standard handle information to use for the new process
#define PS_ATTRIBUTE_STD_HANDLE_INFO \
    PsAttributeValue(PsAttributeStdHandleInfo, FALSE, TRUE, FALSE)
// Specifies the handle list to use for the new process
#define PS_ATTRIBUTE_HANDLE_LIST \
    PsAttributeValue(PsAttributeHandleList, FALSE, TRUE, FALSE)
// Specifies the group affinity to use for the new process
#define PS_ATTRIBUTE_GROUP_AFFINITY \
    PsAttributeValue(PsAttributeGroupAffinity, TRUE, TRUE, FALSE)
// Specifies the preferred NUMA node to use for the new process
#define PS_ATTRIBUTE_PREFERRED_NODE \
    PsAttributeValue(PsAttributePreferredNode, FALSE, TRUE, FALSE)
// Specifies the ideal processor to use for the new process
#define PS_ATTRIBUTE_IDEAL_PROCESSOR \
    PsAttributeValue(PsAttributeIdealProcessor, TRUE, TRUE, FALSE)
// Specifies the process mitigation options to use for the new process
#define PS_ATTRIBUTE_MITIGATION_OPTIONS \
    PsAttributeValue(PsAttributeMitigationOptions, FALSE, TRUE, FALSE)
// Specifies the protection level to use for the new process
#define PS_ATTRIBUTE_PROTECTION_LEVEL \
    PsAttributeValue(PsAttributeProtectionLevel, FALSE, TRUE, FALSE)
// Specifies the UMS thread to associate with the new process
#define PS_ATTRIBUTE_UMS_THREAD \
    PsAttributeValue(PsAttributeUmsThread, TRUE, TRUE, FALSE)
// Specifies whether the new process is a secure process
#define PS_ATTRIBUTE_SECURE_PROCESS \
    PsAttributeValue(PsAttributeSecureProcess, FALSE, TRUE, FALSE)
// Specifies the job list to associate with the new process
#define PS_ATTRIBUTE_JOB_LIST \
    PsAttributeValue(PsAttributeJobList, FALSE, TRUE, FALSE)
// Specifies the child process policy to use for the new process
#define PS_ATTRIBUTE_CHILD_PROCESS_POLICY \
    PsAttributeValue(PsAttributeChildProcessPolicy, FALSE, TRUE, FALSE)
// Specifies the all application packages policy to use for the new process
#define PS_ATTRIBUTE_ALL_APPLICATION_PACKAGES_POLICY \
    PsAttributeValue(PsAttributeAllApplicationPackagesPolicy, FALSE, TRUE, FALSE)
// Specifies the child process should have access to the Win32k subsystem.
#define PS_ATTRIBUTE_WIN32K_FILTER	\
    PsAttributeValue(PsAttributeWin32kFilter, FALSE, TRUE, FALSE)
// Specifies the child process is allowed to claim a specific origin when making a safe file open prompt
#define PS_ATTRIBUTE_SAFE_OPEN_PROMPT_ORIGIN_CLAIM	\
    PsAttributeValue(PsAttributeSafeOpenPromptOriginClaim, FALSE, TRUE, FALSE)
// Specifies the child process is isolated using the BNO framework
#define PS_ATTRIBUTE_BNO_ISOLATION	\
    PsAttributeValue(PsAttributeBnoIsolation, FALSE, TRUE, FALSE)
// Specifies that the child's process desktop application policy
#define PS_ATTRIBUTE_DESKTOP_APP_POLICY	\
    PsAttributeValue(PsAttributeDesktopAppPolicy, FALSE, TRUE, FALSE)


//\
https://github.com/winsiderss/systeminformer/blob/master/phnt/include/ntpsapi.h#L1315

#define PROCESS_CREATE_FLAGS_BREAKAWAY 0x00000001 // NtCreateProcessEx & NtCreateUserProcess
#define PROCESS_CREATE_FLAGS_NO_DEBUG_INHERIT 0x00000002 // NtCreateProcessEx & NtCreateUserProcess
#define PROCESS_CREATE_FLAGS_INHERIT_HANDLES 0x00000004 // NtCreateProcessEx & NtCreateUserProcess
#define PROCESS_CREATE_FLAGS_PROTECTED_PROCESS 0x00000040 // NtCreateUserProcess only
#define PROCESS_CREATE_FLAGS_CREATE_SESSION 0x00000080 // NtCreateProcessEx & NtCreateUserProcess, requires SeLoadDriver
#define PROCESS_CREATE_FLAGS_INHERIT_FROM_PARENT 0x00000100 // NtCreateProcessEx & NtCreateUserProcess
#define PROCESS_CREATE_FLAGS_SUSPENDED 0x00000200 // NtCreateProcessEx & NtCreateUserProcess
#define PROCESS_CREATE_FLAGS_FORCE_BREAKAWAY 0x00000400 // NtCreateProcessEx & NtCreateUserProcess, requires SeTcb
#define PROCESS_CREATE_FLAGS_RELEASE_SECTION 0x00001000 // NtCreateProcessEx & NtCreateUserProcess
#define PROCESS_CREATE_FLAGS_AUXILIARY_PROCESS 0x00008000 // NtCreateProcessEx & NtCreateUserProcess, requires SeTcb
#define PROCESS_CREATE_FLAGS_CREATE_STORE 0x00020000 // NtCreateProcessEx & NtCreateUserProcess
#define PROCESS_CREATE_FLAGS_USE_PROTECTED_ENVIRONMENT 0x00040000 // NtCreateProcessEx & NtCreateUserProcess



//\
https://github.com/winsiderss/systeminformer/blob/master/phnt/include/ntrtl.h#L2688


#define RTL_USER_PROC_PARAMS_NORMALIZED 0x00000001	// indicates that the parameters passed to the process are already in a normalized form
#define RTL_USER_PROC_PROFILE_USER 0x00000002		// enables user-mode profiling for the process
#define RTL_USER_PROC_PROFILE_KERNEL 0x00000004		// enables kernel-mode profiling for the process
#define RTL_USER_PROC_PROFILE_SERVER 0x00000008		// enables server-mode profiling for the process
#define RTL_USER_PROC_RESERVE_1MB 0x00000020		// reserves 1 megabyte (MB) of virtual address space for the process
#define RTL_USER_PROC_RESERVE_16MB 0x00000040		// reserves 16 MB of virtual address space for the process
#define RTL_USER_PROC_CASE_SENSITIVE 0x00000080		// sets the process to be case-sensitive
#define RTL_USER_PROC_DISABLE_HEAP_DECOMMIT 0x00000100	// disables heap decommitting for the process
#define RTL_USER_PROC_DLL_REDIRECTION_LOCAL 0x00001000	// enables local DLL redirection for the process
#define RTL_USER_PROC_APP_MANIFEST_PRESENT 0x00002000	// indicates that an application manifest is present for the process
#define RTL_USER_PROC_IMAGE_KEY_MISSING 0x00004000	// indicates that the image key is missing for the process
#define RTL_USER_PROC_OPTIN_PROCESS 0x00020000		// indicates that the process has opted in to some specific behavior or feature

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

typedef BOOL(WINAPI* t_WinHttpSendRequest)(
	HINTERNET hRequest,
	LPCWSTR   pwszHeaders,
	DWORD     dwHeadersLength,
	LPVOID    lpOptional,
	DWORD     dwOptionalLength,
	DWORD     dwTotalLength,
	DWORD_PTR  dwContext
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
typedef NTSTATUS(NTAPI* t_RtlCreateTimerQueue)(
	PHANDLE NewTimerQueue
	);
typedef NTSTATUS(NTAPI* t_NtCreateEvent) (
	_Out_ PHANDLE EventHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
	_In_ INT_EVENT_TYPE EventType,
	_In_ BOOLEAN InitialState
);

typedef VOID(NTAPI* t_RtlCaptureContext)(
	_Out_ PCONTEXT ContextRecord
);
typedef VOID(NTAPI* RTL_WAITORTIMERCALLBACKFUNC) (
	PVOID lpParameter,
	BOOLEAN TimerOrWaitFired
);

typedef NTSTATUS(NTAPI* t_RtlCreateTimer) (
	HANDLE                      TimerQueue,
	HANDLE* NewTimer,
	RTL_WAITORTIMERCALLBACKFUNC Callback,
	PVOID                       Parameter,
	DWORD                       DueTime,
	DWORD                       Period,
	ULONG                       Flags
);

typedef NTSTATUS(NTAPI* t_RtlRegisterWait) (
	HANDLE* out,
	HANDLE                      handle,
	RTL_WAITORTIMERCALLBACKFUNC callback,
	void*						context,
	ULONG                       milliseconds,
	ULONG                       flags
	);

typedef NTSTATUS(NTAPI* t_RtlDeleteTimerQueue) (
	HANDLE TimerQueueHandle
	);

typedef NTSTATUS(NTAPI* t_RtlCopyMappedMemory)(
	void* pDest,
	const void* pSrc,
	SIZE_T bytesToCopy
	);
typedef NTSTATUS(NTAPI* t_NtWaitForSingleObject) (
	IN HANDLE               ObjectHandle,
	IN BOOLEAN              Alertable,
	IN PLARGE_INTEGER       TimeOut OPTIONAL
	);
typedef NTSTATUS(NTAPI* t_NtSignalAndWaitForSingleObject)(
	IN HANDLE               ObjectToSignal,
	IN HANDLE               WaitableObject,
	IN BOOLEAN              Alertable,
	IN PLARGE_INTEGER       Time OPTIONAL
	);
typedef NTSTATUS(NTAPI* t_NtContinue) (
	PCONTEXT ThreadContext,
	BOOLEAN  RaiseAlert
	);
typedef NTSTATUS(NTAPI* t_NtSetEvent)(
	HANDLE EventHandle,
	PLONG  PreviousState OPTIONAL
	);

typedef NTSTATUS(NTAPI* t_NtSetContextThread)(
	HANDLE          ThreadHandle,
	PCONTEXT        ThreadContext
	);

typedef NTSTATUS(NTAPI* t_NtDuplicateObject)(
	HANDLE      SourceProcessHandle,
	HANDLE      SourceHandle,
	HANDLE      TargetProcessHandle,
	PHANDLE     TargetHandle,
	ACCESS_MASK DesiredAccess,
	ULONG       Attributes,
	ULONG       Options
	);

typedef NTSTATUS(NTAPI* t_LdrGetProcedureAddress)(
	IN HMODULE              ModuleHandle,
	IN PANSI_STRING         FunctionName OPTIONAL,
	IN WORD                 Oridinal OPTIONAL,
	OUT PVOID* FunctionAddress
	);

typedef NTSTATUS(NTAPI* t_NtQueryVirtualMemory)(
	IN HANDLE							ProcessHandle,
	IN PVOID							BaseAddress,
	IN enum MEMORY_INFORMATION_CLASS	MemoryInformationClass,
	OUT PVOID							Buffer,
	IN SIZE_T							Length,
	OUT PSIZE_T							ResultLength OPTIONAL
	);

typedef NTSTATUS(NTAPI* t_NtOpenThread)(
	OUT PHANDLE ThreadHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN P_INT_CLIENT_ID ClientId
	);

typedef NTSTATUS(NTAPI* t_NtAllocateVirtualMemory) (
	IN      HANDLE    ProcessHandle,
	IN OUT	PVOID     *BaseAddress,
	IN		ULONG_PTR ZeroBits,
	IN OUT	PSIZE_T   RegionSize,
	IN      ULONG     AllocationType,
	IN      ULONG     Protect
);
typedef NTSTATUS(NTAPI* t_NtProtectVirtualMemory) (
	IN      HANDLE    ProcessHandle,
	IN OUT	PVOID     *BaseAddress,
	IN OUT	PSIZE_T   NumberOfBytesToProtect,
	IN      ULONG     NewAccessProtection,
	OUT     PULONG    OldAccessProtection
);
typedef NTSTATUS(NTAPI* t_NtCreateThreadEx) (
	OUT PHANDLE hThread,
	IN ACCESS_MASK DesiredAccess,
	IN LPVOID ObjectAttributes,
	IN HANDLE ProcessHandle,
	IN LPTHREAD_START_ROUTINE lpStartAddress,
	IN LPVOID lpParameter,
	IN BOOL CreateSuspended,
	IN ULONG StackZeroBits,
	IN ULONG SizeOfStackCommit,
	IN ULONG SizeOfStackReserve,
	OUT LPVOID lpBytesBuffer
);

typedef NTSTATUS(NTAPI* t_NtSetInformationVirtualMemory)(
	IN HANDLE                           ProcessHandle,
	IN VIRTUAL_MEMORY_INFORMATION_CLASS VmInformationClass,
	IN ULONG_PTR                        NumberOfEntries,
	IN PMEMORY_RANGE_ENTRY              VirtualAddresses,
	IN PVOID                            VmInformation,
	IN ULONG                            VmInformationLength
	);

typedef NTSTATUS(NTAPI* t_NtQueryInformationProcess)(
	IN           HANDLE           ProcessHandle,
	IN           PROCESSINFOCLASS ProcessInformationClass,
	OUT         PVOID            ProcessInformation,
	IN            ULONG            ProcessInformationLength,
	OUT OPTIONAL PULONG           ReturnLength
	);
typedef NTSTATUS(NTAPI* t_NtOpenProcess)(
	OUT PHANDLE ProcessHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN PCLIENT_ID ClientId
	);
typedef NTSTATUS(NTAPI* t_NtReadVirtualMemory)(
	IN HANDLE               ProcessHandle,
	IN PVOID                BaseAddress,
	OUT PVOID               Buffer,
	IN ULONG                NumberOfBytesToRead,
	OUT PULONG              NumberOfBytesReaded OPTIONAL
);

typedef NTSTATUS(NTAPI* t_NtWriteVirtualMemory)(
	IN HANDLE               ProcessHandle,
	IN PVOID                BaseAddress,
	IN PVOID                Buffer,
	IN ULONG                NumberOfBytesToWrite,
	OUT PULONG              NumberOfBytesWritten OPTIONAL
);

typedef NTSTATUS(NTAPI* t_NtCreateUserProcess)(
	OUT         PHANDLE ProcessHandle,
	OUT         PHANDLE ThreadHandle,
	IN          ACCESS_MASK ProcessDesiredAccess,
	IN          ACCESS_MASK ThreadDesiredAccess,
	IN OPTIONAL POBJECT_ATTRIBUTES ProcessObjectAttributes,
	IN OPTIONAL POBJECT_ATTRIBUTES ThreadObjectAttributes,
	IN ULONG    ProcessFlags,                                    // PROCESS_CREATE_FLAGS_*
	IN ULONG    ThreadFlags,                                     // THREAD_CREATE_FLAGS_*
	IN OPTIONAL PRTL_USER_PROCESS_PARAMETERS ProcessParameters,
	IN OUT      PPS_CREATE_INFO CreateInfo,
	IN          PPS_ATTRIBUTE_LIST AttributeList
);

typedef NTSTATUS(NTAPI* t_RtlCreateProcessParametersEx)(
	OUT				PRTL_USER_PROCESS_PARAMETERS*	pProcessParameters,
	IN				PUNICODE_STRING					ImagePathName,
	IN OPTIONAL		PUNICODE_STRING					DllPath,
	IN OPTIONAL		PUNICODE_STRING					CurrentDirectory,
	IN OPTIONAL		PUNICODE_STRING					CommandLine,
	IN OPTIONAL		PVOID							Environment,
	IN OPTIONAL		PUNICODE_STRING					WindowTitle,
	IN OPTIONAL		PUNICODE_STRING					DesktopInfo,
	IN OPTIONAL		PUNICODE_STRING					ShellInfo,
	IN OPTIONAL		PUNICODE_STRING					RuntimeData,
	IN				ULONG							Flags
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
typedef NTSTATUS(NTAPI* t_SystemFunction032)(
	struct USTRING* data,
	const struct USTRING* key
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

typedef HMODULE(WINAPI* t_LoadLibraryA)(
	LPCSTR lpLibFileName
	);

typedef FARPROC(WINAPI* t_GetProcAddress)(
	HMODULE hModule,
	LPCSTR lpProcName
	);

typedef BOOL(WINAPI* t_CreatePipe)(
	OUT          PHANDLE               hReadPipe,
	OUT          PHANDLE               hWritePipe,
	IN OPTIONAL  LPSECURITY_ATTRIBUTES lpPipeAttributes,
	IN           DWORD                 nSize
);

typedef BOOL(WINAPI* t_ReadFile)(
  IN                HANDLE       hFile,
  OUT               LPVOID       lpBuffer,
  IN                DWORD        nNumberOfBytesToRead,
  OUT OPTIONAL     LPDWORD      lpNumberOfBytesRead,
  IN OUT OPTIONAL LPOVERLAPPED lpOverlapped
);

typedef BOOL(WINAPI* t_CreateProcessA)(
	LPCSTR                lpApplicationName,
	LPSTR                 lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL                  bInheritHandles,
	DWORD                 dwCreationFlags,
	LPVOID                lpEnvironment,
	LPCSTR                lpCurrentDirectory,
	LPSTARTUPINFOA        lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
	);

typedef BOOL(WINAPI* t_CreateProcessW)(
IN OPTIONAL      LPCWSTR               lpApplicationName,
IN OUT OPTIONAL LPWSTR                lpCommandLine,
IN OPTIONAL      LPSECURITY_ATTRIBUTES lpProcessAttributes,
IN OPTIONAL		 LPSECURITY_ATTRIBUTES lpThreadAttributes,
IN                BOOL                  bInheritHandles,
IN                DWORD                 dwCreationFlags,
IN OPTIONAL      LPVOID                lpEnvironment,
IN OPTIONAL      LPCWSTR               lpCurrentDirectory,
IN                LPSTARTUPINFOW        lpStartupInfo,
OUT               LPPROCESS_INFORMATION lpProcessInformation
	);

//USER32 APIS
typedef int (WINAPI* t_GetSystemMetrics)(
	int nIndex
	);

typedef BOOL(WINAPI* t_VirtualProtect)(
	LPVOID lpAddress,
	SIZE_T dwSize,
	DWORD  flNewProtect,
	PDWORD lpflOldProtect
	);
typedef DWORD(WINAPI* t_WaitForSingleObjectEx)(
	HANDLE hHandle,
	DWORD  dwMilliseconds,
	BOOL   bAlertable
	);

