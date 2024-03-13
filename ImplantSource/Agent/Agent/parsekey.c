#include "parsekey.h"

#define PUBLIC_KEY_FILE "C:\\Users\\Brendan Ortiz\\Documents\\GOProjcets\\AllSecure\\Config\\ecc_public_key.pem"

int readPublicKey(unsigned char** publicKey, unsigned long* publicKeySize) {
    FILE* file = fopen(PUBLIC_KEY_FILE, "rb");
    if (file == NULL) {
        printf("Error opening public key file\n");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    *publicKeySize = ftell(file);
    fseek(file, 0, SEEK_SET);

    *publicKey = malloc(*publicKeySize);
    fread(*publicKey, 1, *publicKeySize, file);

    fclose(file);
    return 0;
}

void encryptAESKey() {
    
    register_hash(&sha256_desc);
    register_prng(&yarrow_desc);


    // Read the public key from the resource file
    unsigned char* publicKey;
    unsigned long publicKeySize;
    if (readPublicKey(&publicKey, &publicKeySize) != 0) {
        return 1;
    }

    // Generate a random symmetric key for AES encryption
    unsigned char symmetricKey[32];
    if (rand_prime(symmetricKey, sizeof(symmetricKey), find_prng("yarrow"), NULL) != CRYPT_OK) {
        printf("Error generating symmetric key\n");
        return 1;
    }

    // Encrypt the symmetric key with RSA public key
    unsigned char encryptedSymmetricKey[512];  // RSA key size (2048 bits) / 8 = 256 bytes
    unsigned long encryptedSymmetricKeySize = sizeof(encryptedSymmetricKey);
    int result = ecc_encrypt_key(symmetricKey, sizeof(symmetricKey), encryptedSymmetricKey, &encryptedSymmetricKeySize, NULL, 0, NULL, find_prng("yarrow"), find_hash("sha256"), publicKey);
    if (result != CRYPT_OK) {
        printf("Error encrypting symmetric key\n");
        return 1;
    }

  
    free(publicKey);
}