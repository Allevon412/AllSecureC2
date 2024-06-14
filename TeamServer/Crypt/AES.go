package Crypt

import (
	"crypto/aes"
	"crypto/cipher"
	"crypto/rand"
	"log"
)

func AES256CTR(data []byte, AESKey []byte, AESIv []byte) []byte {
	var (
		cryptedBytes = make([]byte, len(data))
	)

	block, err := aes.NewCipher(AESKey)
	if err != nil {
		log.Println("[error] attempting to create new cipher block", err)
		return []byte{}
	}

	stream := cipher.NewCTR(block, AESIv)
	stream.XORKeyStream(cryptedBytes, data)
	return cryptedBytes
}

func AESCTREncrypt(Plaintext []byte) ([]byte, []byte, []byte, error) {
	key := make([]byte, 32)
	iv := make([]byte, 16)

	_, err := rand.Read(key)
	if err != nil {
		log.Println("[error] attempting to generate random key", err)
		return []byte{}, []byte{}, []byte{}, err
	}
	_, err = rand.Read(iv)
	if err != nil {
		log.Println("[error] attempting to generate random iv", err)
		return []byte{}, []byte{}, []byte{}, err
	}

	block, err := aes.NewCipher(key)
	if err != nil {
		log.Println("[error] attempting to create new aes block", err)
		return []byte{}, []byte{}, []byte{}, err
	}

	stream := cipher.NewCTR(block, iv)
	encryptedData := make([]byte, len(Plaintext))
	stream.XORKeyStream(encryptedData, Plaintext)

	return encryptedData, key, iv, nil
}
