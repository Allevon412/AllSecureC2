package Common

type user struct {
	Username string `json:"username"`
	Password string `json:"password"`
}
type jwttoken struct {
	JwtToken string `json:"token"`
}
type UserTableData struct {
	ID       int    `json:"ID"`
	Admin    int    `json:"Admin"`
	Username string `json:"Username"`
	Password string `json:"Password"`
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
