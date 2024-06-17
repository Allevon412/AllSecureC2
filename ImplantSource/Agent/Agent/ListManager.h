#pragma once
#include "agent.h"
#include "Crypto.h"

pHostData AddHost(LPWSTR host, SIZE_T size, INT port);
pHostData SelectHost(DWORD HostRotation);
