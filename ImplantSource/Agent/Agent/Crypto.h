#pragma once
#include <wolfcrypt/aes.h>
#include <Windows.h>
#include "agent.h"


INT AESCTR(BYTE* Buffer, ULONG BufferLength, BYTE* AESKey, UINT32 AesKeySize, BYTE* IV, BOOL Encrypt);
ULONG GenerateRandomNumber(pAgent agent);