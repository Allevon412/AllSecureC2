package Common

import (
	"bytes"
	"crypto/tls"
	"encoding/json"
	"log"
	"net/http"
)

func AuthenticateUser(username, password, server string) (bool, error) {

	var (
		User      User
		Jdata     []byte
		err       error
		resp      *http.Response
		transport http.Transport
		client    http.Client
	)
	transport.TLSClientConfig = &tls.Config{InsecureSkipVerify: true}
	client.Transport = &transport

	User.Username = username
	User.Password = password

	Jdata, err = json.Marshal(User)
	if err != nil {
		log.Println("[error] attempting to marshal json data", err)
		return false, err
	}
	endpoint := "https://" + server + "/AuthenticateUser"

	resp, err = client.Post(endpoint, "application/json", bytes.NewBuffer(Jdata))
	if err != nil {
		log.Println("[error] attmepting post request", err)
		return false, err
	}

	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		log.Println("[error] invalid credentials please try again")
		return false, nil
	}

	return true, nil
}
