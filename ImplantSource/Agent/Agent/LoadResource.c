#include "LoadResource.h"



PCCERT_CONTEXT LoadCertificateFromResource(HMODULE hModule, int ResourceNum) {

	HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(ResourceNum), RT_RCDATA);
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

   
	PCCERT_CONTEXT CertContext = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, DerCert, pbSize);
    if (CertContext == NULL) {
        printf("[!] Error Creating Cert context [%08x]\n", GetLastError());
    }

	FreeResource(hGlobal);
	return CertContext;

}


BYTE* LoadPrivateKeyFromResource(HMODULE hModule, int ResourceNum) {
    HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(ResourceNum), RT_RCDATA);
    if (hResource == NULL) {
        return NULL;
    }

    HGLOBAL hGlobal = LoadResource(hModule, hResource);
    if (hGlobal == NULL) {
        return NULL;
    }

    DWORD dwDataSize = SizeofResource(hModule, hResource);
    const BYTE* data = (const BYTE*)LockResource(hGlobal);
    if (data == NULL) {
        FreeResource(hGlobal);
        return NULL;
    }

    BYTE* DerKey = (BYTE*)malloc(dwDataSize);
    DWORD pbSize = 0;
    if (DerKey == NULL)
    {
        printf("Err attempting to allocate memory");
        return NULL;
    }
    memset(DerKey, 0x00, dwDataSize);

    if (!CryptStringToBinaryA(data, dwDataSize, CRYPT_STRING_BASE64HEADER, NULL, &pbSize, NULL, NULL))
    {
        printf("Failure to obtain the size of data required for buffer. [%08x]\n", GetLastError());
        FreeResource(hGlobal);
        return NULL;
    }

    if (!CryptStringToBinaryA((const char*)data, dwDataSize, CRYPT_STRING_BASE64HEADER, DerKey, &pbSize, NULL, NULL))
    {
        printf("Failure to obtain the size of data required for buffer. [%08x]\n", GetLastError());
        free(DerKey);
        FreeResource(hGlobal);
        return NULL;
    }

    FreeResource(hGlobal);

    return DerKey;
}


/*

*/