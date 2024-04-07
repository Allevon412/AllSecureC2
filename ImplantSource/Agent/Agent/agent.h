#pragma once
#include <Windows.h>

#define AGENT_MAGIC_VALUE 0x0041153C //allsec in l33t speak.

struct Packges; // forward declaration so it can be included in the packages.h header file w/o having to include packages.h here Causing cyclic dependencies.

typedef struct _Agent {
	UINT32 AgentID;
	char EndPoint[256];
	LPSTR OperatingSystem;
	LPSTR UserName;
	LPSTR ComputerName;
	BYTE* AESKey;
	ULONG AESKeySize;
	BYTE* EncryptedAESKey;
	ULONG EncryptedAESKeySize;
	BYTE* IV;
	ULONG IVSize;
	BYTE* EncryptedIV;
	ULONG EncryptedIVSize;

	struct _Package* packages;
}Agent, * pAgent;



enum Agent_Operations {
	INITIALIZE_AGENT = 0,
	EXIT_AGENT,
	SLEEP_AGENT,
	GET_AGENT_JOB,
	CHECKIN_AGENT,
};
