#pragma once
#include <stdio.h>
#include <Windows.h>
#include "tomcrypt.h"

char* CreateAESKey(prng_state* prng);
int ReadPublicKey(unsigned char** publicKey, unsigned long* publicKeySize);
int EncryptAESKey(unsigned char* SymmetricKey, unsigned int SymmetricKeySize, const unsigned char* PublicKey, unsigned long PublicKeySize);
prng_state* CreatePRNGState();
