package ListeningServer

import (
	"crypto/tls"
	"encoding/json"
	"github.com/gorilla/websocket"
	"log"
	"net/http"
	"strings"
)

func CreateWebSocketConnection() (*websocket.Conn, error) {

	var (
		token   []byte
		err     error
		headers http.Header
	)
	
	endpoint := "wss://" + g_clientobj.Server + "/ws"
	token, err = json.Marshal(g_clientobj.Cookie.Token.JwtToken)
	if err != nil {
		log.Println("[error] attempting to marshal JWT token")
		return nil, err
	}
	//set our request authorization header to be our jwt  token.
	headers = make(http.Header)
	headers.Set("Authorization", strings.Trim(string(token), "\""))
	websocket.DefaultDialer.TLSClientConfig = &tls.Config{InsecureSkipVerify: true}
	conn, _, err := websocket.DefaultDialer.Dial(endpoint, headers)
	if err != nil {
		log.Println("[error] attempting to make websocket connection", err)
		return nil, err
	}

	return conn, nil
}

func StartEventHandler(token string, teamserver string) error {
	var err error
	g_clientobj.Cookie.Token.JwtToken = token
	g_clientobj.Server = teamserver
	g_clientobj.Conn, err = CreateWebSocketConnection()
	if err != nil {
		log.Println("[error] attempting to create websocket connection", err)
		return err
	}
	return nil
}
