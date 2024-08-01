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
	UserName     string `json:"username"`
	UserID       int    `json:"UserID"`
}

type ImplantTableData struct {
	ImplantNum  int    `json:"ImplantNum"`
	ImplantName string `json:"ImplantName"`
	InternalIP  string `json:"InternalIP"`
	ExternalIP  string `json:"ExternalIP"`
	User        string `json:"User"`
	Computer    string `json:"Computer"`
	OS          string `json:"OS"`
	PID         int    `json:"PID"`
	Process     string `json:"Process"`
	Health      bool   `json:"Health"`
}

type ImplantCommandData struct {
	Command     string   `json:"Command"`
	ImplantName string   `json:"ImplantName"`
	Args        []string `json:"Args"`
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
