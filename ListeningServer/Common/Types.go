package Common

import (
	"github.com/gin-gonic/gin"
	"github.com/gorilla/websocket"
	"net/http"
)

type (
	HTTPServerConfig struct {
		Name         string
		KillDate     int64
		WorkingHours string
		Method       string
		Port         int
		Secure       bool
		Address      string
	}

	TLSConfig struct {
		Cert []byte
		Key  []byte

		CertPath string
		KeyPath  string
	}

	CookieStruct struct {
		Token jwttoken
	}
)

type (
	NewListener struct {
		Address      string
		Port         int
		Secure       bool
		Engine       *gin.Engine
		Cert, Key    []byte
		ListenerName string
		Path         string
		Jwttoken     string
		TSAddr       string
		TSPort       string
	}

	ListeningServer struct {
		Config *HTTPServerConfig

		GinEngine *gin.Engine
		Server    *http.Server

		TLS TLSConfig

		Active bool
	}

	Client struct {
		Server    string
		Cookie    CookieStruct
		ClientObj http.Client
		Conn      *websocket.Conn
	}
)
