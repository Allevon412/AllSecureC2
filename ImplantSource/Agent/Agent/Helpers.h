#pragma once
#include "Crypto.h"

char* CreateAESKey(prng_state* prng);
char* CreateIV(prng_state* prng);
int ReadPublicKey(unsigned char** publicKey, unsigned long* publicKeySize);
int EncryptAESKey(unsigned char* SymmetricKey, unsigned int SymmetricKeySize, const unsigned char* PublicKey, unsigned long PublicKeySize, unsigned char** OutKey, unsigned long* OutKeySize);
prng_state* CreatePRNGState();
