package Crypt

import (
	"crypto/rand"
	"encoding/hex"
)

func GenerateRandomString(length int) (string, error) {
	RandomBytes := make([]byte, length/2)
	_, err := rand.Read(RandomBytes)
	if err != nil {
		return "", err
	}
	return hex.EncodeToString(RandomBytes), nil
}
