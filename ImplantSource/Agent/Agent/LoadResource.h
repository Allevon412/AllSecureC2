#pragma once
#include <Windows.h>
#include <wincrypt.h>


PCCERT_CONTEXT LoadCertificateFromResource(HMODULE hModule, LPCWSTR ResourceName);
HCRYPTKEY LoadPrivateKeyFromResource(HMODULE hModule, LPCWSTR resourceName);
CHAR* ConvertPEMToBinaryKey(CHAR* pem, DWORD pemSize);