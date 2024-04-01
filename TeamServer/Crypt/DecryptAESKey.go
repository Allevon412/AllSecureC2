package Crypt

import (
	"crypto/aes"
	"crypto/cipher"
	"crypto/ecdsa"
	"crypto/elliptic"
	"crypto/x509"
	"fmt"
	"log"
	"os"
)

func DecryptAESKey(AESKey []byte, AESKeySize uint16, filepath string) error {
	privKeyDer, err := os.ReadFile(filepath + "ecc_private_key.der")
	if err != nil {
		log.Println("[error] attempting to read der bytes from private key file ", err)
		return err
	}

	privKey, err := x509.ParseECPrivateKey(privKeyDer)
	if err != nil {
		log.Println("[error] attempting to decode the EC Private Key ", err)
		return err
	}

	decryptedData, err := ecdhDecrypt(privKey, AESKey)
	if err != nil {
		log.Println("[error] occurred while attempting to decrypt our aeskey ", err)
		return err
	}

	log.Println("Decrypted AESKEY: ", decryptedData)
	for _, byte := range decryptedData {
		fmt.Printf("0x%02X ", byte)
	}
	return nil
}

func ecdhDecrypt(privateKey *ecdsa.PrivateKey, encryptedData []byte) ([]byte, error) {
	// Generate shared secret using ECC private key
	x, _ := elliptic.P256().ScalarMult(privateKey.PublicKey.X, privateKey.PublicKey.Y, privateKey.D.Bytes())

	// Derive symmetric encryption key from shared secret
	symmetricKey := x.Bytes()[:32] // AES-256 key length

	// Decrypt the data using symmetric encryption (AES)
	block, err := aes.NewCipher(symmetricKey)
	if err != nil {
		return nil, err
	}

	// The IV should be unique and unpredictable for each encryption
	iv := encryptedData[:aes.BlockSize]

	// Decrypt the data
	mode := cipher.NewCBCDecrypter(block, iv)
	decryptedData := make([]byte, len(encryptedData)-aes.BlockSize)
	mode.CryptBlocks(decryptedData, encryptedData[aes.BlockSize:])

	// Remove PKCS7 padding
	decryptedData = pkcs7Unpad(decryptedData)

	return decryptedData, nil
}

func pkcs7Unpad(data []byte) []byte {
	length := len(data)
	unpadding := int(data[length-1])
	return data[:length-unpadding]
}
