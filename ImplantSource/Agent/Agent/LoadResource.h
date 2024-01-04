#pragma once
#include <Windows.h>
#include <wincrypt.h>


PCCERT_CONTEXT LoadCertificateFromResource(HMODULE hModule, int ResourceNum);
BYTE* LoadPrivateKeyFromResource(HMODULE hModule, int ResourceNum);