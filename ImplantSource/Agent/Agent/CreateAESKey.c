#include "Helpers.h"

char* CreateAESKey(prng_state* prng) {
   

    // Generate a random symmetric key for AES encryption
    unsigned int symmetricKeySize = 32;
    unsigned char* symmetricKey = (unsigned char*)malloc(sizeof(char) * 32);
    int result;
    result =    sprng_read(symmetricKey, symmetricKeySize, prng);
    if (result != 32) {
        printf("Error generating symmetric key [0x%d]\n", result);
        return 1;
    }
    for (int i = 0; i < symmetricKeySize; i++) {
        printf("0x%02X ", symmetricKey[i]);
    }
    printf("\n");

    return symmetricKey;
}