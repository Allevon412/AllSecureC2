#include "LoadResource.h"



PCCERT_CONTEXT LoadCertificateFromResource(HMODULE hModule, LPCWSTR ResourceName) {

	HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(102), RT_RCDATA);
	if (hResource == NULL) {
        printf("[!] Error Occured Get Last Error [%08x]\n", GetLastError());
		return NULL;
	}

	HGLOBAL hGlobal = LoadResource(hModule, hResource);
	if (hGlobal == NULL)
		return NULL;

	DWORD dwDataSize = SizeofResource(hModule, hResource);
	const BYTE* data = (const BYTE*)LockResource(hGlobal);
	if (data == NULL) {
		FreeResource(hGlobal);
		return NULL;
	}
    DWORD pbSize = 0;
    BYTE* DerCert = (BYTE*)malloc(dwDataSize);
    if (DerCert == NULL)
    {
        printf("Err attempting to allocate memory");
        return NULL;
    }
    memset(DerCert, 0x00, dwDataSize);

    if (!CryptStringToBinaryA(data, dwDataSize, CRYPT_STRING_BASE64HEADER, NULL, &pbSize, NULL, NULL))
    {
        printf("Failure to obtain the size of data required for buffer. [%08x]\n", GetLastError());
        FreeResource(hGlobal);
        return NULL;
    }

   

    if (!CryptStringToBinaryA((const char*)data, dwDataSize, CRYPT_STRING_BASE64HEADER, DerCert, &pbSize, NULL, NULL))
    {
        printf("Failure to obtain the size of data required for buffer. [%08x]\n", GetLastError());
        free(DerCert);
        FreeResource(hGlobal);
        return NULL;
    }

   
	PCCERT_CONTEXT CertContext = CertCreateCertificateContext(X509_ASN_ENCODING, DerCert, pbSize);
    if (CertContext == NULL) {
        printf("[!] Error Creating Cert context [%08x]\n", GetLastError());
    }

	FreeResource(hGlobal);
	return CertContext;

}


HCRYPTKEY LoadPrivateKeyFromResource(HMODULE hModule, LPCWSTR resourceName) {
    HRSRC hResource = FindResource(hModule, resourceName, RT_RCDATA);
    if (hResource == NULL) {
        return NULL;
    }

    HGLOBAL hGlobal = LoadResource(hModule, hResource);
    if (hGlobal == NULL) {
        return NULL;
    }

    DWORD dataSize = SizeofResource(hModule, hResource);
    const BYTE* data = (const BYTE*)LockResource(hGlobal);
    if (data == NULL) {
        FreeResource(hGlobal);
        return NULL;
    }

    // Assuming the private key is in PEM format
    WCHAR* pem = (WCHAR*)data;

    // Convert PEM to binary private key
    WCHAR* binaryKey = ConvertPEMToBinaryKey(pem, dataSize / sizeof(WCHAR));
    if (binaryKey == NULL) {
        FreeResource(hGlobal);
        return NULL;
    }

    HCRYPTPROV hCryptProv;
    if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, 0)) {
        if (GetLastError() != NTE_BAD_KEYSET) {
            FreeResource(hGlobal);
            return NULL;
        }

        // Create a new key container if it does not exist
        if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET)) {
            FreeResource(hGlobal);
            return NULL;
        }
    }

    HCRYPTKEY hCryptKey;
    if (!CryptImportKey(hCryptProv, (BYTE*)binaryKey, wcslen(binaryKey) * sizeof(WCHAR), NULL, 0, &hCryptKey)) {
        //wprintf(L"Error importing private key: %lu\n", GetLastError());
        CryptReleaseContext(hCryptProv, 0);
        return NULL;
    }

    FreeResource(hGlobal);

    return hCryptKey;
}

CHAR* ConvertPEMToBinaryKey(CHAR* pem, DWORD pemSize) {
    // Remove PEM delimiters and line breaks
    CHAR* base64data = pem;
    for (DWORD i = 0; i < pemSize; i++) {
        if (pem[i] != L'-' && pem[i] != L'\r' && pem[i] != L'\n') {
            *base64data++ = pem[i];
        }
    }
    *base64data = L'\0';

    return pem;
}

/* 

*/