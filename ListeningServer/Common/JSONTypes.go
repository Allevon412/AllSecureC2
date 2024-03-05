package Common

type jwttoken struct {
	JwtToken string `json:"token"`
}

type NewWebSocketMessage struct {
	MessageType string `json:"MessageType"`
	Message     string `json:"Message"`
}

type ImplantData struct {
	ImplantNum int    `json:"ImplantNum"`
	ImplantID  string `json:"ImplantID"`
	InternalIP string `json:"InternalIP"`
	ExternalIP string `json:"ExternalIP"`
	User       string `json:"User"`
	Computer   string `json:"Computer"`
	OS         string `json:"OS"`
	PID        int    `json:"PID"`
	Process    string `json:"Process"`
	Health     bool   `json:"Health"`
}
