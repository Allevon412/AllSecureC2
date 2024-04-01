#include "Helpers.h"

int EncryptAESKey(unsigned char* SymmetricKey, unsigned int SymmetricKeySize, const unsigned char* PublicKey, unsigned long PublicKeySize, unsigned char** OutKey, unsigned long* OutKeySize) {
    unsigned int result = 0;
    ecc_key mykey;
    prng_state* pPrng = (prng_state*)malloc(sizeof(prng_state));

    // Encrypt the symmetric key with ECC public key
    unsigned char* EncryptedKey = (unsigned char*)malloc(sizeof(char) * 512);
    unsigned long EncryptedKeySize = 512;

    if (register_prng(&yarrow_desc) == -1) {
        printf("Error registering yarrow\n");
        return -1;
    }

    if ((result = rng_make_prng(128, find_prng("yarrow"), pPrng, NULL)) != CRYPT_OK) {
        printf("Error setting up PRNG, %s\n", error_to_string(result));
        return -1;
    }

    if ((result = ecc_import(PublicKey, PublicKeySize, &mykey)) != CRYPT_OK) {
        printf("error attempting to import public key generated from go: %s\n", error_to_string(result));
        return -1;
    }

    result = ecc_encrypt_key(SymmetricKey, SymmetricKeySize, EncryptedKey, &EncryptedKeySize, pPrng, find_prng("yarrow"), find_hash("sha256"), &mykey);
    if (result != CRYPT_OK) {
        printf("Error encrypting symmetric key %s\n", error_to_string(result));
        return 3;
    }
    for (int i = 0; i < EncryptedKeySize; i++) {
        printf("0x%02X ", EncryptedKey[i]);
    }
    printf("\n");

    *OutKey = EncryptedKey;
    *OutKeySize = EncryptedKeySize;
    free(PublicKey);
    return 0;
}