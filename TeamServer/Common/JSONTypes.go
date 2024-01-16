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
