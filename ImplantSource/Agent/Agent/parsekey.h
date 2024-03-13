#pragma once
#include <stdio.h>
#include "tomcrypt.h"

int readPublicKey(unsigned char** publicKey, unsigned long* publicKeySize);
void encryptAESKey();