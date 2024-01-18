package Common

type user struct {
	Username string `json:"username"`
	Password string `json:"password"`
}
type jwttoken struct {
	JwtToken string `json:"token"`
}
