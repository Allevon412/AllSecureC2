package Types

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

	/*
	 "Name": "AllSecureTS",
	    "Address": "127.0.0.1",
	    "Port": "31337",
	    "DatabasePath" : "C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Database\AllSecure.db",
	    "ConfigPath" : "C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Config\AllSecure.Config",
	    "ProjectDir" : "C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure"
	*/
	TSConfig struct {
		Name         string `json:"Name"`
		Address      string `json:"Address"`
		Port         string `json:"Port"`
		DatabasePath string `json:"DatabasePath"`
		ConfigPath   string `json:"ConfigPath"`
		ProjectDir   string `json:"ProjectDir"`
	}

	ListenerConfig struct {
		Name         string   `json:"Name"`
		ListenerType int      `json:"ListenerType"`
		Method       string   `json:"Method"`
		UserAgent    string   `json:"UserAgent"`
		Headers      string   `json:"Headers"`
		HostHeader   string   `json:"HostHeader"`
		Secure       bool     `json:"Secure"`
		Hosts        []string `json:"Hosts"`
	}

	Client struct {
		ID            string
		Type          ClientType
		Conn          *websocket.Conn
		Username      string
		UserID        int
		RemoteIP      string
		Authenticated bool
		Administrator bool
		Mutex         sync.Mutex
	}

	Endpoint struct {
		Endpoint string
		Function func(ctx *gin.Context)
	}
)

type (
	TeamServer struct {
		TSConfig TSConfig

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

		Endpoints []*Endpoint
		TokenKey  string
	}
)

type (
	ClientType int
)

const (
	ListeningServer ClientType = iota
	ClientApp
)
