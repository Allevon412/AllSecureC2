package Crypt

/*
   #cgo LDFLAGS: -L"C:/Users/Brendan Ortiz/Documents/GOProjcets/AllSecure" -llibtomcrypt
   #include "D:/OffsecTools/crypto/libtomcrypt/src/headers/tomcrypt.h"
   #include <stdio.h>

   unsigned char* DecryptedAESPayload(const unsigned char* AesKey, unsigned long AesKeyLen, unsigned char* IV, unsigned char* Payload, unsigned long sizeOfPayload) {
  		ltc_mp = ltm_desc;
  		register_all_ciphers();
  		register_all_hashes();
  		register_all_prngs();

    int err = 0;

    unsigned char* decryptedPayload = (char*)malloc(sizeof(char) * sizeOfPayload);
    symmetric_CTR ctr;
    if ((err = ctr_start(find_cipher("aes"), IV, AesKey, 32, 0, CTR_COUNTER_LITTLE_ENDIAN, &ctr)) != CRYPT_OK) {
        printf("Error attempting to create counter with provided args %s\n", error_to_string(err));
        return -1;
    }
    if ((err = ctr_decrypt(Payload, decryptedPayload, atoi(argv[1]), &ctr)) != CRYPT_OK) {
        printf("Error attempting to decrypt with provided args\n");
        return -1;
    }

	return decryptedPayload;

*/
//import "C"
