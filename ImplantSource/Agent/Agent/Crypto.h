#pragma once
#include <wolfcrypt/aes.h>
#include <Windows.h>
#include "agent.h"


INT AESCTR(BYTE* Buffer, DWORD BufferLength, BYTE* AESKey, UINT32 AesKeySize, BYTE* IV);
DWORD GenerateRandomNumber();