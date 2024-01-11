package Server

import (
	"AllSecure/TeamServer/Common"
	"AllSecure/TeamServer/Crypt"
	"fmt"
	"github.com/gin-gonic/gin"
	"github.com/gorilla/websocket"
	"log"
	"net/http"
	"os"
	"strings"
)

type TS struct {
	Server Common.TeamServer
}

func (t *TS) ParseConfig(FilePath string) error {

	content, err := os.ReadFile(FilePath)
	fmt.Println(string(content))
	if err != nil {
		log.Fatalln("[error] Error opening file", err)
	}
	parts := strings.Split(string(content), "\n")
	for _, line := range parts {
		line_parts := strings.Split(line, "=")
		switch line_parts[0] {
		case "DatabaseFilePath":
			t.Server.FI.DataBasePath = line_parts[1]
		case "ConfigFilePath":
			t.Server.FI.ConfigFilePath = line_parts[1]
		case "ProjectDir":
			t.Server.FI.ProjectDir = line_parts[1]
		case "TeamServerAddress":
			t.Server.Config.Address = line_parts[1]
		case "TeamServerPort":
			t.Server.Config.Port = line_parts[1]
		default:
			continue
		}
	}
	return nil
}

func (t *TS) HandleRequest(ClientID string) {
	value, isok := t.Server.Clients.Load(ClientID)

	if !isok {
		return
	}

	client := value.(*Common.Client)
	_, NewClient, err := client.Conn.ReadMessage()

	if err != nil {
		log.Println("[error] reading client message", err)
		t.Server.Clients.Delete(ClientID)
		return
	}
	fmt.Println(NewClient)

}

// very similar implmenetaiton to https://github.com/HavocFramework/Havoc/blob/main/teamserver/cmd/server/teamserver.go
func (t *TS) Start() {

	var (
		ServerFinished         chan bool
		DefaultEndPointsParsed chan bool
	)

	//read the configuration file
	currdir, err := os.Getwd()
	if err != nil {
		log.Fatalln("[error] obtaining current directory path")
	}
	parts := strings.Split(currdir, "AllSecure")

	err = t.ParseConfig(parts[0] + "AllSecure\\Config\\AllSecure.Config")
	if err != nil {
		log.Fatalln("[error] reading the configuration file")
	}

	t.Server.GinEngine = gin.Default()

	//add default endpoints in separate routine.
	go func() {
		result := t.AddDefaultEndPoints()
		DefaultEndPointsParsed <- result
	}()

	t.Server.GinEngine.GET("/", func(context *gin.Context) {
		context.Redirect(http.StatusMovedPermanently, "home/")
	})

	t.Server.GinEngine.Static("/home", t.Server.FI.ProjectDir+"\\TeamServer\\Assets\\Index")

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

	t.Server.GinEngine.GET("/ws", func(c *gin.Context) {
		var (
			upgrader      websocket.Upgrader
			WebSocket     *websocket.Conn
			ClientID, err = Crypt.GenerateRandomString(8)
		)
		if err != nil {
			log.Fatalln("[error] could not generate random string when starting team server")
		}

		currdir, err = os.Getwd()
		if err != nil {
			log.Fatalln("[error] could not get the current directory")
		}

		if WebSocket, err = upgrader.Upgrade(c.Writer, c.Request, nil); err != nil {
			c.JSON(http.StatusBadRequest, gin.H{"error": "Failed to set up Websocket"})
			return
		}

		t.Server.Clients.Store(ClientID, &Common.Client{
			Conn:          WebSocket,
			Username:      "",
			RemoteIP:      WebSocket.RemoteAddr().String(),
			Authenticated: false,
		})

		go t.HandleRequest(ClientID)
	})

	go func(Host, Port string) {
		var (
			CertPath = t.Server.FI.ProjectDir + "\\TeamServer" + "\\Assets\\server.cert"
			KeyPath  = t.Server.FI.ProjectDir + "\\TeamServer" + "\\Assets\\server.key"
			Cert     []byte
			Key      []byte
			err      error
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

		if err := t.Server.GinEngine.RunTLS(Host+Port, t.Server.TLS.CertPath, t.Server.TLS.KeyPath); err != nil {
			log.Fatalln("[error] failed to start websocket: ", err)
		}

		ServerFinished <- true

		os.Exit(0)

	}(t.Server.Config.Address, t.Server.Config.Port)

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