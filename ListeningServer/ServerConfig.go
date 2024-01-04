package ListeningServer

import (
	"github.com/gin-gonic/gin"
	"net/http"
)

type (
	HTTPServerConfig struct {
		Name         string
		KillData     int64
		WorkingHours string
		Method       string
		Port         string
		Secure       bool
		Address      string
	}
)

type (
	HTTPServer struct {
		Config HTTPServerConfig

		GinEngine *gin.Engine
		Server    *http.Server

		TLS struct {
			Cert []byte
			Key  []byte

			CertPath string
			KeyPath  string
		}

		Active bool
	}
)
