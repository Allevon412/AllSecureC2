package Crypt

import (
	"crypto/rand"
	"crypto/rsa"
	"crypto/x509"
	"encoding/pem"

	"log"
	"os"
)

func GenerateRSAKeys(filepath, implant_id string) ([]byte, error) {

	privateKey, err := rsa.GenerateKey(rand.Reader, 2048)
	if err != nil {
		log.Println("[error] attmepting to generate the RSA private key object", err)
		return nil, err
	}
	privateKeyDerBytes := x509.MarshalPKCS1PrivateKey(privateKey)

	pemBlockPrivKey := &pem.Block{
		Type:    "RSA PRIVATE KEY",
		Headers: nil,
		Bytes:   privateKeyDerBytes,
	}
	file, err := os.Create(filepath + implant_id + "_rsa_private_key.pem")
	defer file.Close()

	err = pem.Encode(file, pemBlockPrivKey)
	if err != nil {
		log.Println("[error] attempting to create private key file using path ", filepath, err)
		return nil, err
	}
	file, err = os.Create(filepath + implant_id + "_rsa_public_key.der")
	if err != nil {
		return nil, err
	}
	defer file.Close()
	//write public key to file in der format.
	pubKey := privateKey.Public()
	publicKeyDer, err := x509.MarshalPKIXPublicKey(pubKey)
	if err != nil {
		log.Println("[error] attempting to marshal the public key to DER format", err)
		return nil, err
	}
	_, err = file.Write(publicKeyDer)
	if err != nil {
		log.Println("[error] attempting to write the public key to a file", err)
		return nil, err
	}

	return publicKeyDer, nil

}
