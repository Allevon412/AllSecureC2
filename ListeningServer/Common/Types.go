package Common

import (
	"encoding/binary"
	"errors"
	"fmt"
	"github.com/gin-gonic/gin"
	"github.com/gorilla/websocket"
	"net/http"
)

/*
			[ Magic Value  ] 4 bytes
	        [ Agent ID     ] 4 bytes
	        [ Host Name    ] size + bytes
	        [ User Name    ] size + bytes
	       // not included [ Domain       ] size + bytes
			[ Adapter Desc ] size + bytes
	        [ IP Address   ] size + bytes
	        [ Process Name ] size + bytes
	        [ Process ID   ] 4 bytes
	        //not included [ Parent  PID  ] 4 bytes
	        [ Process Arch ] 4 bytes
	        [ Elevated     ] 4 bytes
	        //not included currently [ Base Address ] 8 bytes
	        [ OS Info      ] ( 5 * 4 ) bytes
	        [ OS Arch      ] 4 bytes

*/

type (
	ImplantContext struct {
		Magic_val           []byte
		Agent_id            uint32
		Host_name_length    uint32
		Host_name           string
		User_name_length    uint32
		User_name           string
		Adapter_desc_length uint32
		Adapter_desc        string
		Ip_addr_length      uint32
		Ip_addr             string
		Process_name_length uint32
		Process_name        string
		Process_id          uint32
		Parent_pid          uint32
		Process_arch        uint32
		Elevated            uint32
		Os_info             [5]uint32
		Os_arch             uint32
	}

	Implant struct {
		ImplantName string
		AESKey      []byte
		IV          []byte
		Alive       bool
	}

	Package struct {
		Size          uint32
		MagicVal      []byte
		AgentID       uint32
		CmdID         uint32
		RequestID     uint32
		AgentNameSize uint32
		AgentName     string

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
	TeamServerConfig struct {
		Address string
		Port    string
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

	Queue []interface{}

	AgentCmd struct {
		CmdID      uint32 //command to execute
		RequestID  uint32
		DataBuffer []byte
	}
)

type (
	ListeningServer struct {
		Config HTTPServerConfig
		TS     TeamServerConfig

		GinEngine  *gin.Engine
		HttpServer *http.Server

		TLS TLSConfig

		Active bool

		UserId int
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
			[ Agent Name   ] size + bytes

	     Encrypted Using RSA MetaData:
	        [ Encrypted AES KEY ] ? bytes - calculated at runtime
	        [ Encrypted AES IV  ] ? bytes - calculated at runtime
			[ size of encrypted payload ] 4 bytes - not encrypted

	     Encrypted MetaData:

	        [ Magic Value  ] 4 bytes
	        [ Agent ID     ] 4 bytes
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

const (
	CMD_Register uint32 = iota
	CMD_EXIT
	CMD_SLEEP
	CMD_GET_JOB
	CMD_CHECKIN
	CMD_NO_JOB
)

func (p *Package) UnmarshalHeader(data []byte) error {
	if len(data) < 20 {
		return fmt.Errorf("insufficient data for package unmarshalling")
	}

	p.Size = binary.BigEndian.Uint32(data[:4])
	p.MagicVal = data[4:8]
	p.AgentID = binary.BigEndian.Uint32(data[8:12])
	p.CmdID = binary.BigEndian.Uint32(data[12:16])
	p.RequestID = binary.BigEndian.Uint32(data[16:20])
	p.AgentNameSize = binary.BigEndian.Uint32(data[20:24])
	p.AgentName = string(data[24 : 24+p.AgentNameSize])

	return nil
}

func (p *Package) UnmarshalEncryptedMetaData(data []byte) error {

	var baseindex = 24 + p.AgentNameSize

	//MetaData
	p.EncryptedKeyIvSize = 0x100
	p.EncryptedAESKey = data[baseindex : baseindex+p.EncryptedKeyIvSize]
	p.EncryptedIV = data[baseindex+p.EncryptedKeyIvSize : baseindex+(p.EncryptedKeyIvSize*2)]
	p.EncryptedDataSize = p.Size - (baseindex + p.EncryptedKeyIvSize*2)
	p.EncryptedData = data[baseindex+(p.EncryptedKeyIvSize*2) : baseindex+(p.EncryptedKeyIvSize*2)+p.EncryptedDataSize]

	return nil
}

/*
			[ Magic Value  ] 4 bytes
	        [ Agent ID     ] 4 bytes
	        [ Host Name    ] size + bytes
	        [ User Name    ] size + bytes
	       // not included [ Domain       ] size + bytes
			[ Adapter Desc ] size + bytes
	        [ IP Address   ] size + bytes
	        [ Process Name ] size + bytes
	        [ Process ID   ] 4 bytes
	        //not included [ Parent  PID  ] 4 bytes
	        [ Process Arch ] 4 bytes
	        [ Elevated     ] 4 bytes
	        //not included currently [ Base Address ] 8 bytes
	        [ OS Info      ] ( 5 * 4 ) bytes
	        [ OS Arch      ] 4 bytes
	        //not included currently [ SleepDelay   ] 4 bytes
	        //not included currently [ SleepJitter  ] 4 bytes
	        //not included currently [ Killdate     ] 8 bytes
	        //not included currently [ WorkingHours ] 4 bytes
*/

func (c *ImplantContext) UnmarshalContext(data []byte) error {
	c.Magic_val = data[:4]
	c.Agent_id = binary.BigEndian.Uint32(data[4:8])
	c.Host_name_length = binary.BigEndian.Uint32(data[8:12])
	c.Host_name = string(data[12 : 12+c.Host_name_length])
	c.User_name_length = binary.BigEndian.Uint32(data[12+c.Host_name_length : 16+c.Host_name_length])
	c.User_name = string(data[16+c.Host_name_length : 16+c.Host_name_length+c.User_name_length])
	c.Adapter_desc_length = binary.BigEndian.Uint32(data[16+c.Host_name_length+c.User_name_length : 20+c.Host_name_length+c.User_name_length])
	c.Adapter_desc = string(data[20+c.Host_name_length+c.User_name_length : 20+c.Host_name_length+c.User_name_length+c.Adapter_desc_length])
	c.Ip_addr_length = binary.BigEndian.Uint32(data[20+c.Host_name_length+c.User_name_length+c.Adapter_desc_length : 24+c.Host_name_length+c.User_name_length+c.Adapter_desc_length])
	c.Ip_addr = string(data[24+c.Host_name_length+c.User_name_length+c.Adapter_desc_length : 24+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length])
	c.Process_name_length = binary.BigEndian.Uint32(data[24+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length : 28+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length])
	c.Process_name = string(data[28+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length : 28+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length+c.Process_name_length])
	c.Process_id = binary.BigEndian.Uint32(data[28+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length+c.Process_name_length : 32+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length+c.Process_name_length])
	c.Process_arch = binary.BigEndian.Uint32(data[32+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length+c.Process_name_length : 36+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length+c.Process_name_length])
	c.Elevated = binary.BigEndian.Uint32(data[36+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length+c.Process_name_length : 40+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length+c.Process_name_length])
	c.Os_info[0] = binary.BigEndian.Uint32(data[40+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length+c.Process_name_length : 44+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length+c.Process_name_length])
	c.Os_info[1] = binary.BigEndian.Uint32(data[44+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length+c.Process_name_length : 48+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length+c.Process_name_length])
	c.Os_info[2] = binary.BigEndian.Uint32(data[48+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length+c.Process_name_length : 52+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length+c.Process_name_length])
	c.Os_info[3] = binary.BigEndian.Uint32(data[52+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length+c.Process_name_length : 56+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length+c.Process_name_length])
	c.Os_info[4] = binary.BigEndian.Uint32(data[56+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length+c.Process_name_length : 60+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length+c.Process_name_length])
	c.Os_arch = binary.BigEndian.Uint32(data[60+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length+c.Process_name_length : 64+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length+c.Process_name_length])

	return nil

}

func (q *Queue) Enqueue(value interface{}) {
	*q = append(*q, value)
}

func (q *Queue) Dequeue() (interface{}, error) {
	if len(*q) == 0 {
		return nil, errors.New("[error] Queue is empty")
	}
	element := (*q)[0]
	*q = (*q)[1:]
	return element, nil
}

func (a *AgentCmd) MarshalAgentCmd() []byte {
	var data = make([]byte, 8)
	binary.LittleEndian.PutUint32(data[:4], a.CmdID)
	binary.LittleEndian.PutUint32(data[4:8], a.RequestID)
	data = append(data, a.DataBuffer...)
	return data
}
