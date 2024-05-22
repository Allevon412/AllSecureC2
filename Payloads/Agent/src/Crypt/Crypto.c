#include "Crypto.h"

//TODO rewrite this to use AES encryption mechanism from maldev academy.
INT AESEncrypt(BYTE* Buffer, ULONG BufferLength, pAgent pAgent) {
	Aes aes;
	int err;

	if ((err = wc_AesSetKey(&aes, pAgent->AESKey, pAgent->AESKeySize, pAgent->IV, AES_ENCRYPTION)) != 0) {
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
