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
    

    prng_state prng;

    if (yarrow_start(&prng) != CRYPT_OK) {
        printf("[Error] attempting to start yarrow PRNG\n");
        return 1;
    }
    unsigned char entropy[32];
    time_t currentTime = time(NULL);
    memcpy(entropy, &currentTime, sizeof(currentTime));
    if (yarrow_add_entropy(entropy, sizeof(entropy), &prng) != CRYPT_OK)
    {
        printf("[error] attempting to add entrypt to yarrow");
        return 1;
    }

    yarrow_ready(&prng);

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
    int result;
    result = yarrow_read(&symmetricKey, sizeof(symmetricKey), &prng);
    if (result != 32) {
        printf("Error generating symmetric key [0x%d]\n", result);
        return 1;
    }

    

    // Encrypt the symmetric key with ECC public key
    unsigned char encryptedSymmetricKey[512];  // ECC key size (2048 bits) / 8 = 256 bytes
    unsigned long encryptedSymmetricKeySize = sizeof(encryptedSymmetricKey);
    result = ecc_encrypt_key(symmetricKey, sizeof(symmetricKey), encryptedSymmetricKey, &encryptedSymmetricKeySize, &prng, find_prng("yarrow"), find_hash("sha256"), publicKey);
    if (result != CRYPT_OK) {
        printf("Error encrypting symmetric key\n");
        return 1;
    }
    yarrow_done(&prng);
  
    free(publicKey);
}