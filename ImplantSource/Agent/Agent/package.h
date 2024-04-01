#pragma once
typedef struct _Package {
	//Header
	unsigned int Size;
	unsigned char MagicVal[4];
	unsigned char AgentID[4];
	unsigned char CmdID[4];
	unsigned char RequestID[4];

	//MetaData
	unsigned short EncryptedAESKeySize;
	unsigned char EncryptedAESKey[150];
	

}Package, * pPackage;
