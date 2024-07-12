#pragma once
#include "../agent/agent.h"
#include "../crypt/Crypto.h"

pHostData AddHost(LPWSTR host, SIZE_T size, INT port);
pHostData SelectHost(DWORD HostRotation);
