package Common

type jwttoken struct {
	JwtToken string `json:"token"`
}

type NewWebSocketMessage struct {
	MessageType string `json:"MessageType"`
	Message     string `json:"Message"`
}

type ImplantData struct {
	ImplantId   uint32 `json:"ImplantNum"`
	ImplantName string `json:"ImplantName"`
	InternalIP  string `json:"InternalIP"`
	ExternalIP  string `json:"ExternalIP"`
	User        string `json:"User"`
	Computer    string `json:"Computer"`
	OS          string `json:"OS"`
	PID         uint32 `json:"PID"`
	Process     string `json:"Process"`
	Health      bool   `json:"Health"`
}

type ImplantCommandData struct {
	Command     string   `json:"Command"`
	ImplantName string   `json:"ImplantName"`
	Args        []string `json:"Args"`
}
