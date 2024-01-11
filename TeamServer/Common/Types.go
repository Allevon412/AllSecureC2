package Common

import (
	"github.com/gin-gonic/gin"
	"github.com/gorilla/websocket"
	"net/http"
	"sync"
)

type (
	HTTPServerConfig struct {
		Name         string
		KillDate     int64
		WorkingHours string
		Method       string
		Port         string
		Secure       bool
		Address      string
	}

	Client struct {
		ID            string
		Conn          *websocket.Conn
		Username      string
		RemoteIP      string
		Authenticated bool
		Mutex         sync.Mutex
		SessionId     string
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
	}
)
