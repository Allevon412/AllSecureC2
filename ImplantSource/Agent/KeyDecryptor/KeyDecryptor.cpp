// KeyDecryptor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include "tomcrypt.h"

int ReadPrivKey(unsigned char** privKey, unsigned long* privKeySize, const char* filepath);

int main(int argc, char* argv[])
{

    if (argc < 4) {
        printf("[error] not enough arguments provided\n");
        printf("Example: \n");
        printf(".\KeyDecryptor.exe <base64 encoded encrypted key> <Path to private key file> <size of encrypted key>\n");
        return -1;
    }
    ltc_mp = ltm_desc;
    register_all_ciphers();
    register_all_hashes();
    register_all_prngs();

    int err = 0;
    unsigned char decodedkey[250];
    unsigned long decodedKeySize = sizeof(decodedkey);

    unsigned char* privKey;
    unsigned long privKeySize;

    unsigned char decryptedKey[250];
    unsigned long decryptedKeySize = sizeof(decryptedKey);

    if ((err = base64_decode(argv[1], strlen(argv[1]), decodedkey, &decodedKeySize) != CRYPT_OK)) {
        printf("[error] attmepting to base64 decode the aeskey. %s\n", error_to_string(err));
        return -1;
    }

    ReadPrivKey(&privKey, &privKeySize, argv[2]);
    
    ecc_key importedKey;
    if ((err = ecc_import(privKey, privKeySize, &importedKey)) != CRYPT_OK) {
        printf("[error] performing private key import  %s\n", error_to_string(err));
        return -1;
    }
    
    if ((err = ecc_decrypt_key(decodedkey, atoi(argv[3]), decryptedKey, &decryptedKeySize, &importedKey)) != CRYPT_OK) {
        printf("[error] attemtping to decrypt key %s\n", error_to_string(err));
        return -1;
    }
    
    for (int i = 0; i < decryptedKeySize; i++) {
        printf("%c", decryptedKey[i]);
    }
    printf("\n");
    return 0;
}


int ReadPrivKey(unsigned char** privKey, unsigned long* privKeySize, const char* filepath) {
    FILE* file = NULL;
    fopen_s(&file, filepath, "rb");
    if (file == NULL) {
        printf("Error opening public key file\n");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    *privKeySize = ftell(file);
    fseek(file, 0, SEEK_SET);

    *privKey = (unsigned char*) malloc(*privKeySize);
    fread(*privKey, 1, *privKeySize, file);

    fclose(file);
    return 0;
}

