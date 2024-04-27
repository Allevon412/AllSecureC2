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
		EncryptedKeyIvSize uint32
		EncryptedAESKey    []byte
		EncryptedIV        []byte

		//payload
		EncryptedData     []byte
		EncryptedDataSize uint32
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

/*

Taken from havoc as a basic model.
     Unencrypted Header (if specified):
        [ SIZE         ] 4 bytes
        [ Magic Value  ] 4 bytes
        [ Agent ID     ] 4 bytes
        [ COMMAND ID   ] 4 bytes
        [ Request ID   ] 4 bytes

     Encrypted Using ECC MetaData:
        [ Encrypted AES KEY Len ] 4 bytes - not encrypted
        [ Encrypted AES KEY ] ? bytes - calculated at runtime
        [ Encrypted AES IV Len ] 4 bytes - not encrypted
        [ Encrypted AES IV  ] ? bytes - calculated at runtime
		[ size of encrypted payload ] 4 bytes - not encrypted

     Encrypted MetaData:

        [ Magic Value  ] 4 bytes
        [ Demon ID     ] 4 bytes
        [ Host Name    ] size + bytes
        [ User Name    ] size + bytes
        [ Domain       ] size + bytes
        [ IP Address   ] 16 bytes?
        [ Process Name ] size + bytes
        [ Process ID   ] 4 bytes
        [ Parent  PID  ] 4 bytes
        [ Process Arch ] 4 bytes
        [ Elevated     ] 4 bytes
        [ Base Address ] 8 bytes
        [ OS Info      ] ( 5 * 4 ) bytes
        [ OS Arch      ] 4 bytes
        [ SleepDelay   ] 4 bytes
        [ SleepJitter  ] 4 bytes
        [ Killdate     ] 8 bytes
        [ WorkingHours ] 4 bytes
        ..... more
        [ Optional     ] Eg: Pivots, Extra data about the host or network etc.


	Encrypted Payload :

		[ encrypted payload ] ? bytes - calculated at runtime
*/

func (p *Package) UnmarshalBinary(data []byte) error {
	if len(data) < 172 {
		return fmt.Errorf("insufficient data for package unmarshalling")
	}

	p.Size = binary.BigEndian.Uint32(data[:4])
	p.MagicVal = data[4:8]
	p.AgentID = data[8:12]
	p.CmdID = data[12:16]
	p.RequestID = data[16:20]

	//MetaData
	p.EncryptedKeyIvSize = 0x100
	p.EncryptedAESKey = data[20 : 20+p.EncryptedKeyIvSize]
	p.EncryptedIV = data[20+p.EncryptedKeyIvSize : 20+(p.EncryptedKeyIvSize*2)]
	p.EncryptedDataSize = p.Size - (20 + p.EncryptedKeyIvSize*2)
	p.EncryptedData = data[20+(p.EncryptedKeyIvSize*2) : +20+(p.EncryptedKeyIvSize*2)+p.EncryptedDataSize]

	return nil
}
