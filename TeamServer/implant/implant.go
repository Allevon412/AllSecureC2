package implant

import (
	"AllSecure/TeamServer/Crypt"
	"crypto/ecdsa"
	"log"
	"os"
)

type Agent struct {
	Id      string
	CurrCmd string
}

func GenerateNewImplant() {
	var (
		NewAgent   Agent
		err        error
		PrivateKey *ecdsa.PrivateKey
	)

	NewAgent.Id, err = Crypt.GenerateRandomString(16)
	if err != nil {
		log.Fatalln(err)
	}
	ImplantDir := "C:\\Users\\Brendan Ortiz\\Documents\\GOProjcets\\AllSecure\\CreatedImplants\\" + NewAgent.Id
	err = os.Mkdir(ImplantDir, 0777)
	if err != nil {
		log.Fatalln(err)
	}
	KeyDir := ImplantDir + "\\CryptoKeys"
	err = os.Mkdir(KeyDir, 0777)
	if err != nil {
		log.Fatalln(err)
	}

	PrivateKey, err = Crypt.GeneratePrivateKey(KeyDir, NewAgent.Id)
	if err != nil {
		log.Fatalln(err)
	}

	_, err = Crypt.GenerateSelfSignedCert(KeyDir, NewAgent.Id, PrivateKey, "127.0.0.1")
	if err != nil {
		log.Fatalln(err)
	}
}
