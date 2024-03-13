package Crypt

import (
	"crypto/ecdsa"
	"crypto/elliptic"
	"crypto/rand"
	"crypto/x509"
	"encoding/pem"
	"log"
	"os"
)

func GenerateECCKeys() (*ecdsa.PrivateKey, error) {
	// Generate a private key
	privateKey, err := ecdsa.GenerateKey(elliptic.P256(), rand.Reader)
	if err != nil {
		return nil, err
	}

	return privateKey, nil
}

func SaveKeysToPEMFile(privateKey *ecdsa.PrivateKey, filepath string) error {
	var publicKeyBytes []byte

	//obtain public key from pirvate key
	pubKey := privateKey.Public()

	// Encode private key to PEM format
	privateKeyBytes, err := x509.MarshalECPrivateKey(privateKey)
	if err != nil {
		return err
	}

	// Create a PEM block for the private key
	pemBlock := &pem.Block{
		Type:  "EC PRIVATE KEY",
		Bytes: privateKeyBytes,
	}

	// Write PEM block to a file
	file, err := os.Create(filepath + "ecc_private_key.pem")
	if err != nil {
		return err
	}
	defer file.Close()

	err = pem.Encode(file, pemBlock)
	if err != nil {
		log.Println("[error] attempting to create private key file using path ", filepath, err)
		return err
	}
	//use public key to save to file.
	publicKeyBytes, err = x509.MarshalPKIXPublicKey(pubKey)
	pemBlock = &pem.Block{
		Type:  "EC PUBLIC KEY",
		Bytes: publicKeyBytes,
	}
	file, err = os.Create(filepath + "ecc_public_key.pem")
	if err != nil {
		return err
	}
	defer file.Close()

	return pem.Encode(file, pemBlock)
}
