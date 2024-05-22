package Crypt

import (
	"crypto/aes"
	"crypto/cipher"
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
