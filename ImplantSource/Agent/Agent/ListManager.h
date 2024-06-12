#pragma once
#include "agent.h"
#include "Crypto.h"

pHostData AddHost(pAgent agent, LPWSTR host, SIZE_T size, INT port);
pHostData SelectHost(pAgent agent, DWORD HostRotation);
