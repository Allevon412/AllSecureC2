#pragma once
#include <Windows.h>
#include "tomcrypt.h"
#include "agent.h"


INT AESEncrypt(BYTE* Buffer, ULONG BufferLength, pAgent pAgent);