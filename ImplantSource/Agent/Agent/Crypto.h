#pragma once
#include <wolfcrypt/aes.h>
#include <Windows.h>
#include "agent.h"


INT AESEncrypt(BYTE* Buffer, ULONG BufferLength, pAgent pAgent);