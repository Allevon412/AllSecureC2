package Crypt

import (
	"crypto/ecdsa"
	"crypto/elliptic"
	"crypto/rand"
	"crypto/x509"
	"crypto/x509/pkix"
	"encoding/pem"
	"log"
	"math/big"
	"os"
	"time"
)

func GeneratePrivateKey(FilePath string, AgentId string) (*ecdsa.PrivateKey, error) {
	FullFilePath := FilePath + "\\" + "priv-" + AgentId + ".pem"
	PrivateKey, err := ecdsa.GenerateKey(elliptic.P384(), rand.Reader)
	if err != nil {
		log.Println("[!] Error generating private key: ", err)
		return nil, err
	}

	PrivateKeyBytes, err := x509.MarshalECPrivateKey(PrivateKey)
	if err != nil {
		log.Println("[!] Error encoding private key: ", err)
		return nil, err
	}

	log.Println("[+] Creating private key file at loc ", FullFilePath)
	hPrivateKeyFile, err := os.Create(FullFilePath)
	if err != nil {
		log.Println("[!] Error creating private key file: ", err)
		return nil, err
	}
	defer hPrivateKeyFile.Close()

	err = pem.Encode(hPrivateKeyFile, &pem.Block{Type: "EC PRIVATE KEY", Bytes: PrivateKeyBytes})
	if err != nil {
		log.Println("[!] Error writing private key to file: ", err)
		return nil, err
	}
	return PrivateKey, nil
}

func GenerateSelfSignedCert(FilePath string, AgentId string, PrivateKey *ecdsa.PrivateKey, ServerName string) (Path string, err error) {

	FullFilePath := FilePath + "\\" + "pub-" + AgentId + ".pem"
	FullFilePathDer := FilePath + "\\" + "pub-" + AgentId + ".der"
	CertificateTemplate := x509.Certificate{
		SerialNumber:          big.NewInt(1),
		Subject:               pkix.Name{CommonName: ServerName},
		NotBefore:             time.Now(),
		NotAfter:              time.Now().Add(365 * 24 * time.Hour), // valid for 1 year
		KeyUsage:              x509.KeyUsageKeyEncipherment | x509.KeyUsageDigitalSignature,
		ExtKeyUsage:           []x509.ExtKeyUsage{x509.ExtKeyUsageServerAuth},
		BasicConstraintsValid: true,
	}
	CertificateDer, err := x509.CreateCertificate(rand.Reader, &CertificateTemplate, &CertificateTemplate,
		&PrivateKey.PublicKey, PrivateKey)

	os.WriteFile(FullFilePathDer, CertificateDer, 0777)

	if err != nil {
		log.Println("[!] Error creating certificate: ", err)
		return "", err
	}

	hCertificateFile, err := os.Create(FullFilePath)
	if err != nil {
		log.Print("[!] Error creating public key: ", err)
		return "", err
	}
	defer hCertificateFile.Close()

	err = pem.Encode(hCertificateFile, &pem.Block{Type: "CERTIFICATE", Bytes: CertificateDer})
	if err != nil {
		log.Println("[!] Error writing certificate to file: ", err)
		return "", err
	}
	return FullFilePath, nil
}
