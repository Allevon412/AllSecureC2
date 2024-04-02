#include "Helpers.h"

#define PUBLIC_KEY_FILE "C:\\Users\\Brendan Ortiz\\Documents\\GOProjcets\\AllSecure\\Config\\test_implant_ecc_public_key.der"

int ReadPublicKey(unsigned char** publicKey, unsigned long* publicKeySize) {

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

