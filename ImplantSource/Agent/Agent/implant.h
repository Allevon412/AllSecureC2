#pragma once

typedef struct _Agent {
	char Id[256];
	char EndPoint[256];
	LPSTR OperatingSystem;
	LPSTR UserName;
	LPSTR ComputerName;
	unsigned char* AESKey;
	unsigned long AESKeySize;
	unsigned char* EncryptedAESKey;
	unsigned long EncryptedAESKeySize;

}Agent, *pAgent;

