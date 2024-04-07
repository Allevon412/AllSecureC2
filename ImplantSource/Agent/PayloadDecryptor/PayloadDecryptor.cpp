// PayloadDecryptor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include "tomcrypt.h"

int main(int argc, char* argv[])
{

    if (argc < 6) {
        printf("[error] not enough arguments provided\n");
        printf("Example: \n");
        printf(".\PayloadDecrypter.exe <size of encrypted payload> <base64 encoded encrypted payload> <b64 AESKey> <b64 IV>\n");
        return -1;
    }
    ltc_mp = ltm_desc;
    register_all_ciphers();
    register_all_hashes();
    register_all_prngs();

    int err = 0;
    BYTE* decodedAESkey = (BYTE*)LocalAlloc(LMEM_FIXED| LMEM_ZEROINIT, sizeof(BYTE) * atoi(argv[5]));
    unsigned long decodedAESKeySize;

    BYTE* decodedIV = (BYTE*)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(BYTE) * atoi(argv[7]));
    unsigned long decodedIVSize;

    BYTE* decodedPayload = (BYTE*)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(BYTE) * atoi(argv[1]));
    unsigned long decodedPayloadSize;

    unsigned char decryptedPayload[250];

    if ((err = base64url_sane_decode(argv[4], strlen(argv[4]), decodedAESkey, &decodedAESKeySize) != CRYPT_OK)) {
        printf("[error] attmepting to base64 decode the aeskey. %s\n", error_to_string(err));
        return -1;
    }

    if ((err = base64url_sane_decode(argv[6], strlen(argv[6]), decodedIV, &decodedIVSize) != CRYPT_OK)) {
        printf("[error] attmepting to base64 decode the aeskey. %s\n", error_to_string(err));
        return -1;
    }

    if ((err = base64url_sane_decode(argv[2], strlen(argv[2]), decodedPayload, &decodedPayloadSize) != CRYPT_OK)) {
        printf("[error] attmepting to base64 decode the aeskey. %s\n", error_to_string(err));
        return -1;
    }
    printf("Decoded Key: ");
    for (int i = 0; i < decodedAESKeySize; i++) {
        printf("0x%02X ", decodedAESkey[i]);
    }
    printf("\n");
    printf("Decoded IV: ");
    for (int i = 0; i < decodedIVSize; i++) {
        printf("0x%02X ", decodedIV[i]);
    }
    printf("\n");
    printf("Decoded Payload: ");
    for (int i = 0; i < atoi(argv[1]); i++) {
        printf("0x%02X ", decodedPayload[i]);
    }
    printf("\n");

    symmetric_CTR ctr;
    if ((err = ctr_start(find_cipher("aes"), decodedIV, decodedAESkey, 32, 0, CTR_COUNTER_LITTLE_ENDIAN, &ctr)) != CRYPT_OK) {
        printf("Error attempting to create counter with provided args %s\n", error_to_string(err));
        return -1;
    }
    if ((err = ctr_decrypt(decodedPayload, decryptedPayload, atoi(argv[1]), &ctr)) != CRYPT_OK) {
        printf("Error attempting to decrypt with provided args\n");
        return -1;
    }

    for (int i = 0; i < atoi(argv[1]); i++) {
        printf("%c", decryptedPayload[i]);
    }

    printf("\n");
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
