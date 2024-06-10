package Crypt

import (
	"AllSecure/ListeningServer/Common"
	"crypto/aes"
	"crypto/cipher"
	"crypto/rand"
	"log"
)

func AESDecryptPayload(DataPackage Common.Package, filepath string) ([]byte, error) {
	var (
		err             error
		DecryptedAESKey []byte
		DecryptedIV     []byte
	)

	DecryptedAESKey, err = DecryptKeyIV(DataPackage.EncryptedAESKey, filepath)
	if err != nil {
		return []byte{}, err
	}

	DecryptedIV, err = DecryptKeyIV(DataPackage.EncryptedIV, filepath)
	if err != nil {
		return []byte{}, err
	}

	block, err := aes.NewCipher(DecryptedAESKey)
	if err != nil {
		log.Println("[error] attempting to create new aes block", err)
		return []byte{}, err
	}
	stream := cipher.NewCTR(block, DecryptedIV)
	plaintext := make([]byte, DataPackage.EncryptedDataSize)
	stream.XORKeyStream(plaintext, DataPackage.EncryptedData)

	return plaintext, nil

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
