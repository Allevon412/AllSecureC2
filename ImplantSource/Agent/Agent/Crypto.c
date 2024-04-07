#include "Crypto.h"

//TODO rewrite this to use AES encryption mechanism from maldev academy.
INT AESEncrypt(BYTE* Buffer, ULONG BufferLength, pAgent pAgent) {
	symmetric_CTR ctr;
	int err;

	if ((err = ctr_start(
		find_cipher("aes"),				//AES CIpher
		pAgent->IV,					//IV
		pAgent->AESKey,				//KEY
		pAgent->AESKeySize,			//KEYSIZE
		0,								//Default number of rounds
		CTR_COUNTER_LITTLE_ENDIAN,		//CTR Mode
		&ctr							//CTR context
	)) != CRYPT_OK) {
		printf("[error] attempting to initialize ctr. %s\n", error_to_string(err));
		return err;
	}

	if ((err = ctr_encrypt(Buffer, Buffer, BufferLength, &ctr)) != CRYPT_OK) {
		printf("[error] attempting to encrypt our package buffer %s\n", error_to_string(err));
		return err;
	}


	ctr_done(&ctr);
	return CRYPT_OK;
}
