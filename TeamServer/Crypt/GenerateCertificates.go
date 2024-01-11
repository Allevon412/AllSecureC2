package Crypt

import (
	"bytes"
	"crypto/ecdsa"
	"crypto/elliptic"
	"crypto/rand"
	"crypto/rsa"
	"crypto/x509"
	"crypto/x509/pkix"
	"encoding/binary"
	"encoding/pem"
	"fmt"
	"log"
	"math/big"
	"net"
	"os"
	"strings"
	"time"

	insecureRand "math/rand"
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

//copied from:  https://github.com/HavocFramework/Havoc/blob/main/teamserver/pkg/common/certs/https.go#L300
//which was copied from: https://github.com/BishopFox/sliver/blob/master/server/certs/https.go

const (
	// HTTPSCA - Directory containing operator certificates
	HTTPSCA = "https"

	// RSAKeySize - Default size of RSA keys in bits
	RSAKeySize = 2048 // This is plenty 4096 is overkill

	// Certs are valid for ~3 Years, minus up to 1 year from Now()
	validFor = 3 * (365 * 24 * time.Hour)

	// ECCKey - Namespace for ECC keys
	ECCKey = "ecc"

	// RSAKey - Namespace for RSA keys
	RSAKey = "rsa"
)

var (
	// State -> Localities -> Street Addresses
	states = map[string]map[string][]string{
		"": {
			"": {""},
		},
		"Arizona": {
			"Phoenix":    {""},
			"Mesa":       {""},
			"Scottsdale": {""},
			"Chandler":   {""},
		},
		"California": {
			"San Francisco": {"", "Golden Gate Bridge"},
			"Oakland":       {""},
			"Berkeley":      {""},
			"Palo Alto":     {""},
			"Los Angeles":   {""},
			"San Diego":     {""},
			"San Jose":      {""},
		},
		"Colorado": {
			"Denver":       {""},
			"Boulder":      {""},
			"Aurora":       {""},
			"Fort Collins": {""},
		},
		"Connecticut": {
			"New Haven":  {""},
			"Bridgeport": {""},
			"Stamford":   {""},
			"Norwalk":    {""},
		},
		"Washington": {
			"Seattle": {""},
			"Tacoma":  {""},
			"Olympia": {""},
			"Spokane": {""},
		},
		"Florida": {
			"Miami":        {""},
			"Orlando":      {""},
			"Tampa":        {""},
			"Jacksonville": {""},
		},
		"Illinois": {
			"Chicago":    {""},
			"Aurora":     {""},
			"Naperville": {""},
			"Peoria":     {""},
		},
	}

	orgNames = []string{
		"",
		"ACME",
		"Partners",
		"Tech",
		"Cloud",
		"Synergy",
		"Test",
		"Debug",
	}
	orgSuffixes = []string{
		"",
		"co",
		"llc",
		"inc",
		"corp",
		"ltd",
	}
)

func randomState() string {
	keys := make([]string, 0, len(states))
	for k := range states {
		keys = append(keys, k)
	}
	return keys[insecureRand.Intn(len(keys))]
}

func randomLocality(state string) string {
	locales := states[state]
	keys := make([]string, 0, len(locales))
	for k := range locales {
		keys = append(keys, k)
	}
	return keys[insecureRand.Intn(len(keys))]
}

func randomStreetAddress(state string, locality string) string {
	addresses := states[state][locality]
	return addresses[insecureRand.Intn(len(addresses))]
}

func randomProvinceLocalityStreetAddress() ([]string, []string, []string) {
	state := randomState()
	locality := randomLocality(state)
	streetAddress := randomStreetAddress(state, locality)
	return []string{state}, []string{locality}, []string{streetAddress}
}

func randomPostalCode() []string {
	switch insecureRand.Intn(1) {
	case 0:
		return []string{fmt.Sprintf("%d", insecureRand.Intn(8000)+1000)}
	default:
		return []string{}
	}
}

func randomSubject(commonName string) *pkix.Name {
	province, locale, street := randomProvinceLocalityStreetAddress()
	return &pkix.Name{
		Organization:  randomOrganization(),
		Country:       []string{"US"},
		Province:      province,
		Locality:      locale,
		StreetAddress: street,
		PostalCode:    randomPostalCode(),
		CommonName:    commonName,
	}
}

func randomOrganization() []string {
	name := orgNames[insecureRand.Intn(len(orgNames))]
	suffix := orgSuffixes[insecureRand.Intn(len(orgSuffixes))]
	switch insecureRand.Intn(4) {
	case 0:
		return []string{strings.TrimSpace(strings.ToLower(name + " " + suffix))}
	case 1:
		return []string{strings.TrimSpace(strings.ToUpper(name + " " + suffix))}
	case 2:
		return []string{strings.TrimSpace(strings.Title(fmt.Sprintf("%s %s", name, suffix)))}

	default:
		return []string{}
	}
}

func publicKey(priv interface{}) interface{} {
	switch k := priv.(type) {
	case *rsa.PrivateKey:
		return &k.PublicKey
	case *ecdsa.PrivateKey:
		return &k.PublicKey
	default:
		return nil
	}
}

func randomInt(max int) int {
	buf := make([]byte, 4)
	rand.Read(buf)
	i := binary.LittleEndian.Uint32(buf)
	return int(i) % max
}

func pemBlockForKey(priv interface{}) *pem.Block {
	switch key := priv.(type) {
	case *rsa.PrivateKey:
		data := x509.MarshalPKCS1PrivateKey(key)
		return &pem.Block{Type: "RSA PRIVATE KEY", Bytes: data}
	case *ecdsa.PrivateKey:
		data, err := x509.MarshalECPrivateKey(key)
		if err != nil {
			log.Println(fmt.Sprintf("Unable to marshal ECDSA private key: %v", err))
		}
		return &pem.Block{Type: "EC PRIVATE KEY", Bytes: data}
	default:
		return nil
	}
}

func generateCertificate(caType string, subject pkix.Name, isCA bool, isClient bool, privateKey interface{}) ([]byte, []byte) {

	// Valid times, subtract random days from .Now()
	notBefore := time.Now()
	days := randomInt(365) * -1 // Within -1 year
	notBefore = notBefore.AddDate(0, 0, days)
	notAfter := notBefore.Add(validFor)
	log.Println(fmt.Sprintf("Valid from %v to %v", notBefore, notAfter))

	// Serial number
	serialNumberLimit := new(big.Int).Lsh(big.NewInt(1), 128)
	serialNumber, _ := rand.Int(rand.Reader, serialNumberLimit)
	log.Println(fmt.Sprintf("Serial Number: %d", serialNumber))

	var keyUsage = x509.KeyUsageKeyEncipherment | x509.KeyUsageDigitalSignature
	var extKeyUsage []x509.ExtKeyUsage

	if isCA {
		log.Println("Authority certificate")
		keyUsage = x509.KeyUsageCertSign | x509.KeyUsageKeyEncipherment | x509.KeyUsageDigitalSignature
		extKeyUsage = []x509.ExtKeyUsage{
			x509.ExtKeyUsageServerAuth,
			x509.ExtKeyUsageClientAuth,
		}
	} else if isClient {
		log.Println("Client authentication certificate")
		extKeyUsage = []x509.ExtKeyUsage{x509.ExtKeyUsageClientAuth}
	} else {
		log.Println("Server authentication certificate")
		extKeyUsage = []x509.ExtKeyUsage{x509.ExtKeyUsageServerAuth}
	}
	log.Println(fmt.Sprintf("ExtKeyUsage = %v", extKeyUsage))

	// Certificate template
	template := x509.Certificate{
		SerialNumber:          serialNumber,
		Subject:               subject,
		NotBefore:             notBefore,
		NotAfter:              notAfter,
		KeyUsage:              keyUsage,
		ExtKeyUsage:           extKeyUsage,
		BasicConstraintsValid: isCA,
	}

	if !isClient {
		// Host or IP address
		if ip := net.ParseIP(subject.CommonName); ip != nil {
			log.Println(fmt.Sprintf("Certificate authenticates IP address: %v", ip))
			template.IPAddresses = append(template.IPAddresses, ip)
		} else {
			log.Println(fmt.Sprintf("Certificate authenticates host: %v", subject.CommonName))
			template.DNSNames = append(template.DNSNames, subject.CommonName)
		}
	} else {
		log.Println(fmt.Sprintf("Client certificate authenticates CN: %v", subject.CommonName))
	}

	// Sign certificate or self-sign if CA
	var certErr error
	var derBytes []byte

	if isCA {
		log.Println("Certificate is an AUTHORITY")
		template.IsCA = true
		template.KeyUsage |= x509.KeyUsageCertSign
		derBytes, certErr = x509.CreateCertificate(rand.Reader, &template, &template, publicKey(privateKey), privateKey)
	}

	if certErr != nil {
		// We maybe don't want this to be fatal, but it should basically never happen afaik
		log.Println(fmt.Sprintf("Failed to create certificate: %s", certErr))
	}

	// Encode certificate and key
	certOut := bytes.NewBuffer([]byte{})
	pem.Encode(certOut, &pem.Block{Type: "CERTIFICATE", Bytes: derBytes})

	keyOut := bytes.NewBuffer([]byte{})
	pem.Encode(keyOut, pemBlockForKey(privateKey))

	return certOut.Bytes(), keyOut.Bytes()
}

// HTTPSGenerateRSACertificate - Generate a server certificate signed with a given CA
func HTTPSGenerateRSACertificate(host string) ([]byte, []byte, error) {
	log.Println(fmt.Sprintf("Generating TLS certificate (RSA) for '%s' ...", host))

	var privateKey interface{}
	var err error

	// Generate private key
	privateKey, err = rsa.GenerateKey(rand.Reader, RSAKeySize)
	if err != nil {
		log.Println("Failed to generate private key %s", err)
		return nil, nil, err
	}
	subject := randomSubject(host)
	cert, key := generateCertificate(HTTPSCA, (*subject), true, false, privateKey)
	// err = saveCertificate(HTTPSCA, RSAKey, host, cert, key)
	return cert, key, err
}
