package Common

import (
	"github.com/gin-gonic/gin"
	"github.com/gorilla/websocket"
	"net/http"
	"sync"
)

const (
	HTTP_SERVER = 1
	SMB_SERVER  = 2
)

type (
	DBUser struct {
		ID       int
		Username string
		Password string
		Admin    int
	}

	HTTPServerConfig struct {
		Name         string
		KillDate     int64
		WorkingHours string
		Method       string
		UserAgent    string
		Headers      []string
		HostHeader   string
		Secure       bool
		HostRotation string
		Hosts        []string
	}

	Client struct {
		ID            string
		Conn          *websocket.Conn
		Username      string
		UserID        int
		RemoteIP      string
		Authenticated bool
		Administrator bool
		Mutex         sync.Mutex
	}

	FileInfo struct {
		DataBasePath   string
		ConfigFilePath string
		ProjectDir     string
	}

	Endpoint struct {
		Endpoint string
		Function func(ctx *gin.Context)
	}
)

type (
	TeamServer struct {
		Config  HTTPServerConfig
		Clients sync.Map //sync safe capability for multiple go routines to handle multiple clients. havoc framework inspired

		GinEngine *gin.Engine
		Server    *http.Server

		TLS struct {
			Cert []byte
			Key  []byte

			CertPath string
			KeyPath  string
		}

		Active bool
		FI     FileInfo

		Endpoints []*Endpoint
		TokenKey  string
	}
)
