package Server

import (
	"AllSecure/TeamServer/Common"
	"AllSecure/TeamServer/EndPoints"
	"github.com/gin-gonic/gin"
	"io"
	"log"
	"strings"
)

func AuthenticateUser(ctx *gin.Context) {
	var (
		username string
		password string
	)

	content, err := io.ReadAll(ctx.Request.Body)
	if err != nil {
		log.Println("[error] attempting to read post body.")
	}
	defer ctx.Request.Body.Close()

	parts := strings.Split(string(content), "&")

	for _, part := range parts {
		parts2 := strings.Split(part, "=")
		for j, part2 := range parts2 {
			switch part2 {
			case "username":
				username = parts2[j+1]
			case "password":
				password = parts2[j+1]
			default:
				continue
			}
		}
	}

	if EndPoints.AuthenticateUser(username, password) {
		ctx.Status(200)
		return
	}
	ctx.Status(500)
	return
}

func (t *TS) AddAuthenticateUserEndpoint() bool {

	AuthUser := &Common.Endpoint{
		Endpoint: "AuthenticateUser",
		Function: AuthenticateUser,
	}

	return t.AddEndPoint(AuthUser)
}

func (t *TS) AddDefaultEndPoints() bool {
	t.AddAuthenticateUserEndpoint()
	return true
}
