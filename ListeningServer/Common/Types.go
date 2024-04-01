package Common

import (
	"encoding/binary"
	"fmt"
	"github.com/gin-gonic/gin"
	"github.com/gorilla/websocket"
	"net/http"
)

type (
	Package struct {
		Size      uint32
		MagicVal  []byte
		AgentID   []byte
		CmdID     []byte
		RequestID []byte

		//MetaData
		EncryptedAESKeySize uint16
		EncryptedAESKey     []byte
	}

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

func (p *Package) UnmarshalBinary(data []byte) error {
	if len(data) < 172 {
		return fmt.Errorf("insufficient data for package unmarshalling")
	}

	p.Size = binary.LittleEndian.Uint32(data[:4])
	p.MagicVal = data[4:8]
	p.AgentID = data[8:12]
	p.CmdID = data[12:16]
	p.RequestID = data[16:20]

	//MetaData
	p.EncryptedAESKeySize = binary.LittleEndian.Uint16(data[20:22])
	p.EncryptedAESKey = data[22:]
	return nil
}
