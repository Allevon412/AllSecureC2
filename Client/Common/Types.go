package Common

import (
	"github.com/gorilla/websocket"
	"net/http"
)

type ListenerConfig struct {
	Name         string   `json:"Name"`
	ListenerType int      `json:"ListenerType"`
	Method       string   `json:"Method"`
	UserAgent    string   `json:"UserAgent"`
	Headers      string   `json:"Headers"`
	HostHeader   string   `json:"HostHeader"`
	Secure       bool     `json:"Secure"`
	Hosts        []string `json:"Hosts"`
}

type (
	CookieStruct struct {
		Token jwttoken
	}

	Client struct {
		Server    string
		Username  string
		Password  string
		Admin     bool
		Cookie    CookieStruct
		ClientObj http.Client
		Conn      *websocket.Conn
	}

	ImplantConfig struct {
		Arch                 int            `json:"Arch"`
		Name                 string         `json:"Name"`
		KillDate             int64          `json:"KillDate"`
		WorkingHours         string         `json:"WorkingHours"`
		HostRotation         string         `json:"HostRotation"`
		ListenerConfig       ListenerConfig `json:"ListenerConfig"`
		InjectionType        string         `json:"InjectionType"`
		MemoryEncryptionType string         `json:"MemoryEncryptionType"`
		Format               string         `json:"Format"`
		Sleep                int            `json:"Sleep"`
		Jitter               int            `json:"Jitter"`
	}
)
