package Common

import (
	"bytes"
	"crypto/tls"
	"encoding/json"
	"errors"
	"io"
	"log"
	"net/http"
	"strings"
)

func PerformHTTPReq(clientobj *Client, endpoint string, body []byte) ([]byte, error) {
	var (
		token     []byte
		err       error
		req       *http.Request
		resp      *http.Response
		transport http.Transport
		client    http.Client
		data      []byte
	)

	//setup the http tls configuration
	transport.TLSClientConfig = &tls.Config{InsecureSkipVerify: true}
	client.Transport = &transport

	//create a new request that our client will perform.
	req, err = http.NewRequest("POST", endpoint, bytes.NewBuffer(body))
	if err != nil {
		log.Println("[error] attempting to generate a new HTTP request with the specified parameters", err)
	}
	//marshal token into it's byte form.
	token, err = json.Marshal(clientobj.Cookie.Token.JwtToken)
	//set our request authorization header to be our jwt  token.
	req.Header.Set("Authorization", strings.Trim(string(token), "\""))

	//perform the request.
	resp, err = client.Do(req)
	if err != nil {
		log.Println("[error] attempting to perform request", err)
		return nil, err
	}

	defer resp.Body.Close()
	status := resp.StatusCode
	if status != http.StatusOK {
		log.Println("[error] in attempting to get user data. status: ", status)
		return nil, errors.New("invalid status code")
	}

	data, err = io.ReadAll(resp.Body)
	if err != nil {
		log.Println("[error] attempting to read resp body")
		return nil, err
	}
	return data, nil
}
