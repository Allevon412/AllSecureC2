
#include "Helpers.h"
#include "localcstd.h"
//TODO: generate a random AES key from server & IV.
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
    //TODO REMOVE THE OUTPUT FOR NON DEBUG PURPOSES.

    return symmetricKey;
}

char* CreateIV(prng_state* prng) {
    unsigned int IVSize = 16;
    BYTE* IV = (BYTE*)malloc(sizeof(BYTE) * IVSize);
    
    int result;
    if ((result = sprng_read(IV, IVSize, prng)) != IVSize) {
        printf("[error] attempting to generate randomized IV [%d]\n", result);
        return -1;
    }

    //TODO REMOVE THE OUTPUT FOR NON DEBUG PURPOSES.
    
    return IV;
}