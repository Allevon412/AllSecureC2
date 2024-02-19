package Common

import "github.com/dgrijalva/jwt-go"

type User struct {
	Username string `json:"username"`
	Password string `json:"password"`
}
type JWTClaims struct {
	UserID        int    `json:"user_id"`
	Username      string `json:"username"`
	Administrator bool   `json:"admin"`
	jwt.StandardClaims
}

type NewUser struct {
	Username string `json:"Username"`
	Password string `json:"Password"`
	Admin    bool   `json:"Admin"`
}

type WebSocketMessage struct {
	MessageType string `json:"MessageType"`
	Message     string `json:"Message"`
}

type ListenerData struct {
	ListenerName string `json:"ListenerName"`
	Protocol     string `json:"Protocol"`
	HOST         string `json:"HOST"`
	PortBind     int    `json:"PortBind"`
	PortConn     string `json:"PortConn"`
	UserName     string `json:"username"`
	UserID       int    `json:"UserID"`
}
