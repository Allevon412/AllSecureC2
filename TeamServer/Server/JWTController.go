package Server

import (
	"AllSecure/TeamServer/Common/Types"
	"fmt"
	"github.com/dgrijalva/jwt-go"
	"time"
)

func (t *TS) CreateToken(userID int, username string, Admin bool) (string, error) {

	claims := Types.JWTClaims{
		UserID:        userID,
		Username:      username,
		Administrator: Admin,
		StandardClaims: jwt.StandardClaims{
			ExpiresAt: time.Now().Add(time.Hour * 1).Unix(), // Token expiration time
			IssuedAt:  time.Now().Unix(),
		},
	}

	token := jwt.NewWithClaims(jwt.SigningMethodHS256, claims)
	signedToken, err := token.SignedString([]byte(t.Server.TokenKey))
	if err != nil {
		return "", err
	}

	return signedToken, nil
}

func (t *TS) ParseToken(TokenString string) (*Types.JWTClaims, error) {
	token, err := jwt.ParseWithClaims(TokenString, &Types.JWTClaims{}, func(token *jwt.Token) (interface{}, error) {
		return []byte(t.Server.TokenKey), nil
	})
	if err != nil {
		return nil, err
	}

	claims, ok := token.Claims.(*Types.JWTClaims)
	if !ok {
		return nil, fmt.Errorf("Couldn't parse claims")
	}

	return claims, nil
}
