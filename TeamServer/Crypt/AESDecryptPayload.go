package Crypt

import (
	"AllSecure/ListeningServer/Common"
	"crypto/aes"
	"crypto/cipher"
	"log"
)

func AESDecryptPayload(DataPackage Common.Package, filepath string) ([]byte, error) {
	var (
		err             error
		DecryptedAESKey []byte
		DecryptedIV     []byte
	)

	DecryptedAESKey, err = DecryptKeyIV(DataPackage.EncryptedAESKey, filepath, DataPackage.AgentName)
	if err != nil {
		return []byte{}, err
	}

	DecryptedIV, err = DecryptKeyIV(DataPackage.EncryptedIV, filepath, DataPackage.AgentName)
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
