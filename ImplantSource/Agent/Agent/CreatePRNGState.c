#include "Helpers.h"

/*

prng_state* CreatePRNGState() {
    prng_state* prng = (prng_state*)malloc(sizeof(prng_state));

    if (sprng_start(prng) != CRYPT_OK) {
        printf("[Error] attempting to start sprng PRNG\n");
        return 1;
    }
    unsigned char entropy[32];
    time_t currentTime = time(NULL);
    memcpy(entropy, &currentTime, sizeof(currentTime));
    if (sprng_add_entropy(entropy, sizeof(entropy), prng) != CRYPT_OK)
    {
        printf("[error] attempting to add entrypt to yarrow");
        return 1;
    }

    sprng_ready(prng);

    return prng;
}
*/