package Common

type user struct {
	Username string `json:"username"`
	Password string `json:"password"`
}
type jwttoken struct {
	JwtToken string `json:"token"`
}
type UserTableData struct {
	ID              int    `json:"ID"`
	Admin           int    `json:"Admin"`
	Username        string `json:"Username"`
	Password        string `json:"Password"`
	ActiveListeners string `json:"ActiveListeners"`
	ActiveImplants  string `json:"ActiveImplants"`
}
type ListenerTableData struct {
	ListenerName string `json:"ListenerName"`
	Protocol     string `json:"Protocol"`
	HOST         string `json:"HOST"`
	PortBind     int    `json:"PortBind"`
	PortConn     string `json:"PortConn"`
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
