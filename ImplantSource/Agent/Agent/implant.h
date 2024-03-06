#pragma once

typedef struct _Agent {
	char Id[256];
	char EndPoint[256];
	LPSTR OperatingSystem;
	LPSTR UserName;
	LPSTR ComputerName;
}Agent, *pAgent;

