package Crypt

import (
	"AllSecure/TeamServer/Common/Types"
	"crypto/ecdsa"
	"crypto/elliptic"
	"crypto/rand"
	"crypto/x509"
	"encoding/asn1"
	"encoding/pem"
	"log"
	"os"
)

// TODO unused file remove later.
func GenerateECCKeys() (*ecdsa.PrivateKey, error) {
	// Generate a private key
	privateKey, err := ecdsa.GenerateKey(elliptic.P256(), rand.Reader)
	if err != nil {
		return nil, err
	}

	return privateKey, nil
}

func SaveKeysToDERFile(privateKey *ecdsa.PrivateKey, filepath string, implant_id string) error {
	curve := elliptic.P256()
	privateKey, err := ecdsa.GenerateKey(curve, rand.Reader)
	if err != nil {
		return err
	}

	privateKeyDerBytes, err := x509.MarshalECPrivateKey(privateKey)

	// Write PEM block to a file
	pemBlockPrivKey := &pem.Block{
		Type:    "EC PRIVATE KEY",
		Headers: nil,
		Bytes:   privateKeyDerBytes,
	}

	file, err := os.Create(filepath + implant_id + "_ecc_private_key.pem")
	if err != nil {
		return err
	}
	defer file.Close()

	err = pem.Encode(file, pemBlockPrivKey)
	if err != nil {
		log.Println("[error] attempting to create private key file using path ", filepath, err)
		return err
	}

	file, err = os.Create(filepath + implant_id + "_ecc_private_key.der")
	if err != nil {
		return err
	}
	defer file.Close()

	privKey := Types.ECCPrivateKey{
		Flags: asn1.BitString{
			Bytes:     []byte{0x80}, //0x80 = 128 or most significant bit = 1
			BitLength: 1,
		},
		KeySize: curve.Params().BitSize / 8,
		PubkeyX: privateKey.PublicKey.X,
		PubKeyY: privateKey.PublicKey.Y,
		SecretK: privateKey.D,
	}
	privateKeyDerBytes, err = asn1.Marshal(privKey)

	_, err = file.Write(privateKeyDerBytes)
	if err != nil {
		log.Println("[error] attempting to create private key der file.", err)
		return err
	}

	pubKey := Types.ECCPublicKey{
		Flags: asn1.BitString{
			Bytes:     []byte{0x00},
			BitLength: 1,
		},
		KeySize: curve.Params().BitSize / 8,
		PubKeyX: privateKey.PublicKey.X,
		PubKeyY: privateKey.PublicKey.Y,
	}

	file, err = os.Create(filepath + implant_id + "_ecc_public_key.der")
	if err != nil {
		return err
	}
	defer file.Close()

	publicKeyDerBytes, err := asn1.Marshal(pubKey)

	if err != nil {
		log.Println("[error] attempting to create public key file.")
		return err
	}
	_, err = file.Write(publicKeyDerBytes)

	return err //pem.Encode(file, pemBlock)
}
