#include "Helpers.h"

int EncryptAESKey(unsigned char* SymmetricKey, unsigned int SymmetricKeySize, const unsigned char* PublicKey, unsigned long PublicKeySize) {
    unsigned int result = 0;
    ecc_key mykey;
    prng_state prng;

    // Encrypt the symmetric key with ECC public key
    unsigned char EncryptedSymmetricKey[512];  // ECC key size (2048 bits) / 8 = 256 bytes
    unsigned long EncryptedSymmetricKeySize = sizeof(EncryptedSymmetricKey);

    if (register_prng(&yarrow_desc) == -1) {
        printf("Error registering yarrow\n");
        return -1;
    }

    if ((result = rng_make_prng(128, find_prng("yarrow"), &prng, NULL)) != CRYPT_OK) {
        printf("Error setting up PRNG, %s\n", error_to_string(result));
        return -1;
    }

    if ((result = ecc_import(PublicKey, PublicKeySize, &mykey)) != CRYPT_OK) {
        printf("error attempting to import public key generated from go: %s\n", error_to_string(result));
        return -1;
    }

    result = ecc_encrypt_key(SymmetricKey, SymmetricKeySize, EncryptedSymmetricKey, &EncryptedSymmetricKeySize, &prng, find_prng("yarrow"), find_hash("sha256"), &mykey);
    if (result != CRYPT_OK) {
        printf("Error encrypting symmetric key %s\n", error_to_string(result));
        return 3;
    }


    free(PublicKey);
    return 0;
}