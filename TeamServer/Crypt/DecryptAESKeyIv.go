package Crypt

/*
   #cgo LDFLAGS: -L"C:/Users/Brendan Ortiz/Documents/GOProjcets/AllSecure" -llibtomcrypt
   #include "D:/OffsecTools/crypto/libtomcrypt/src/headers/tomcrypt.h"
   #include <stdio.h>

   unsigned char* DecryptAESKey(const unsigned char* privKey, unsigned long keyLen, unsigned char* EncAesKey, unsigned long AesKeyLen) {
  		ltc_mp = ltm_desc;
  		register_all_ciphers();
  		register_all_hashes();
  		register_all_prngs();

		ecc_key importedKey;
		unsigned char* DecryptedKey = (char*)malloc(sizeof(char) * AesKeyLen);
		unsigned long OutLen = AesKeyLen;
		int err;

		if((err = ecc_import(privKey, keyLen, &importedKey)) != CRYPT_OK) {
			printf("Error importing ECC key: %s\n", error_to_string(err));
				return NULL;
		}

		if((err = ecc_decrypt_key(EncAesKey, AesKeyLen, DecryptedKey, &OutLen, &importedKey)) != CRYPT_OK) {
			printf("Error decrypting AES key, in CGO FUNC: %s\n", error_to_string(err));
			return NULL;
		}

	return DecryptedKey;
   }
*/
import "C"
import (
	"AllSecure/TeamServer/Common"

	"crypto/x509"
	"encoding/asn1"
	"encoding/pem"
	"fmt"
	"log"
	"os"
	"unsafe"
)

func DecryptKeyIV(AESKey []byte, AESKeySize uint32, filepath string) ([]byte, error) {

	DecryptedKeyOrIv, err := DecryptAESKeyIvInGo(AESKey, AESKeySize, filepath+"Config\\test_implant_ecc_private_key.pem")
	if err != nil {
		log.Println("[error] attempting to decrypt AES key in Go", err)
	}

	return DecryptedKeyOrIv, nil
}

func DecryptAESKeyIvInGo(AESKey []byte, AESKeySize uint32, filepath string) ([]byte, error) {
	privateKeyPem, err := os.ReadFile(filepath)
	if err != nil {
		return []byte{}, err
	}

	block, _ := pem.Decode(privateKeyPem)

	privateKey, err := x509.ParseECPrivateKey(block.Bytes)
	if err != nil {
		return []byte{}, err
	}

	//setup the ECC private key in format recognized by TomCrypt.
	EccPrivKey := Common.ECCPrivateKey{
		Flags:   asn1.BitString{[]byte{0x80}, 1},
		KeySize: privateKey.Params().BitSize / 8,
		PubkeyX: privateKey.PublicKey.X,
		PubKeyY: privateKey.PublicKey.Y,
		SecretK: privateKey.D,
	}
	privateKeyBytes, err := asn1.Marshal(EccPrivKey)
	if err != nil {
		return []byte{}, err
	}

	decryptedKeyC := C.DecryptAESKey((*C.uchar)(unsafe.Pointer(&privateKeyBytes[0])), C.ulong(len(privateKeyBytes)),
		(*C.uchar)(unsafe.Pointer(&AESKey[0])), (C.ulong)(AESKeySize))
	if decryptedKeyC == nil {
		return []byte{}, fmt.Errorf("error decrypting AES key")
	}
	decryptedKey := C.GoBytes(unsafe.Pointer(decryptedKeyC), C.int(AESKeySize))
	C.free(unsafe.Pointer(decryptedKeyC))
	return decryptedKey, nil
}
