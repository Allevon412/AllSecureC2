package Common

import (
	"github.com/gin-gonic/gin"
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
)

type (
	ListeningServer struct {
		Config *HTTPServerConfig

		GinEngine *gin.Engine
		Server    *http.Server

		TLS TLSConfig

		Active bool
	}
)
