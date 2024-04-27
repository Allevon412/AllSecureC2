package Crypt

import (
	"crypto/rsa"

	"crypto/x509"
	"encoding/pem"
	"log"
	"os"
)

func DecryptKeyIV(AESKey []byte, filepath string) ([]byte, error) {

	DecryptedKeyOrIv, err := DecryptAESKeyIvInGo(AESKey, filepath+"Config\\test_implant_rsa_private_key.pem")
	if err != nil {
		log.Println("[error] attempting to decrypt AES key in Go", err)
	}

	return DecryptedKeyOrIv, nil
}

func DecryptAESKeyIvInGo(AESKey []byte, filepath string) ([]byte, error) {

	privateKeyPem, err := os.ReadFile(filepath)
	if err != nil {
		return []byte{}, err
	}
	block, _ := pem.Decode(privateKeyPem)
	privateKey, err := x509.ParsePKCS1PrivateKey(block.Bytes)

	plaintext, err := rsa.DecryptPKCS1v15(nil, privateKey, AESKey)
	if err != nil {
		return []byte{}, err
	}

	return plaintext, nil
}
