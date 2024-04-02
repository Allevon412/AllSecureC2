package Crypt

import (
	"AllSecure/TeamServer/Common"
	"encoding/base64"
	"log"
	"os/exec"
	"strconv"
)

func DecryptAESKey(AESKey []byte, AESKeySize uint16, filepath string) ([]byte, error) {
	formattedKey := Common.RemoveNullBytes(AESKey)
	EncodedKey := base64.StdEncoding.EncodeToString(formattedKey)
	log.Println("Cmd String: ", filepath+"KeyDecryptor.exe", EncodedKey, filepath+"Config\\test_implant_ecc_private_key.der", strconv.Itoa(int(AESKeySize)))

	cmd := exec.Command(filepath+"KeyDecryptor.exe",
		EncodedKey, filepath+"Config\\test_implant_ecc_private_key.der", strconv.Itoa(int(AESKeySize)))

	decryptedKey, err := cmd.Output()
	if err != nil {
		log.Println("[error] attempting to obtain the output of the command", err)
		return []byte{}, err
	}

	return decryptedKey, nil
}
