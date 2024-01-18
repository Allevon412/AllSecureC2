package Common

import (
	"bytes"
	"crypto/tls"
	"encoding/json"
	"io"
	"log"
	"net/http"
)

func AuthenticateUser(clientobj *Client) (bool, error) {

	var (
		User      user
		Jdata     []byte
		bodybytes []byte
		err       error
		resp      *http.Response
		transport http.Transport
		client    http.Client
	)

	transport.TLSClientConfig = &tls.Config{InsecureSkipVerify: true}
	client.Transport = &transport

	User.Username = clientobj.Username
	User.Password = clientobj.Password

	Jdata, err = json.Marshal(User)
	if err != nil {
		log.Println("[error] attempting to marshal json data", err)
		return false, err
	}
	endpoint := "https://" + clientobj.Server + "/AuthenticateUser"

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

	bodybytes, err = io.ReadAll(resp.Body)
	if err != nil {
		log.Printf("[error] could not read jwt token from body err: %s\n", err)
		return false, err
	}

	err = json.Unmarshal(bodybytes, &clientobj.Cookie.Token)
	if err != nil {
		log.Println("[error] unmarshalling the jwt token.", err)
		return false, err
	}

	return true, nil
}

func RetrieveUserData() {

}
