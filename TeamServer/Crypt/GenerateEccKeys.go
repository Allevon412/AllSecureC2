package Crypt

import (
	"AllSecure/TeamServer/Common"
	"crypto/ecdsa"
	"crypto/elliptic"
	"crypto/rand"
	"encoding/asn1"
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

func SaveKeysToDERFile(privateKey *ecdsa.PrivateKey, filepath string) error {
	curve := elliptic.P256()
	privateKey, err := ecdsa.GenerateKey(curve, rand.Reader)
	if err != nil {
		return err
	}

	privateKeyBytes, err := asn1.Marshal(privateKey)

	// Write PEM block to a file
	file, err := os.Create(filepath + "ecc_private_key.der")
	if err != nil {
		return err
	}
	defer file.Close()

	err = os.WriteFile(filepath+"ecc_private_key.der", privateKeyBytes, 0644)
	if err != nil {
		log.Println("[error] attempting to create private key file using path ", filepath, err)
		return err
	}

	pubKey := Common.ECCPublicKey{
		Flags: asn1.BitString{
			Bytes:     []byte{0x00},
			BitLength: 1,
		},
		KeySize: curve.Params().BitSize / 8,
		PubKeyX: privateKey.PublicKey.X,
		PubKeyY: privateKey.PublicKey.Y,
	}

	file, err = os.Create(filepath + "ecc_public_key.der")
	if err != nil {
		return err
	}
	defer file.Close()

	log.Println(pubKey)
	publicKeyDerBytes, err := asn1.Marshal(pubKey)
	log.Println(publicKeyDerBytes)
	if err != nil {
		log.Println("[error] attempting to create public key file.")
		return err
	}
	_, err = file.Write(publicKeyDerBytes)

	return err //pem.Encode(file, pemBlock)
}
