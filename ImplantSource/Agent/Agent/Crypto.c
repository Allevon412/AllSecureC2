#include "Crypto.h"

//TODO rewrite this to use AES encryption mechanism from maldev academy.
INT AESCTR(BYTE* Buffer, ULONG BufferLength, BYTE* AESKey, UINT32 AesKeySize, BYTE* IV) {
	Aes aes;
	int err;

	if ((err = wc_AesSetKey(&aes, AESKey, AesKeySize, IV, AES_ENCRYPTION)) != 0) {
		printf("[error] attempting to initialize AES context\n");
		return err;
	}


	if((err = wc_AesCtrEncrypt(&aes, Buffer, Buffer, BufferLength)) != 0) {
		printf("[error] attempting to encrypt our package buffer\n");
		return err;
	}

	wc_AesFree(&aes);

	return 0;
}

ULONG GenerateRandomNumber(pAgent agent) {
	ULONG Seed = 0;
	Seed = agent->pNtGetTickCount();
	Seed = agent->pRtlRandomEx(&Seed);
	Seed = agent->pRtlRandomEx(&Seed);

	Seed = (Seed % (LONG_MAX - 2 + 1)) + 2;

	return Seed % 2 == 0 ? Seed : Seed + 1;
}