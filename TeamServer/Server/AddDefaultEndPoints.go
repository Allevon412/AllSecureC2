package Server

import (
	"AllSecure/TeamServer/Common"
	"AllSecure/TeamServer/EndPoints"
	"github.com/gin-gonic/gin"
	"log"
	"net/http"
)

var DatabasePath string

// TODO Create functioanlity to read JWT tokens.
func (t *TS) AuthenticateUser(ctx *gin.Context) {
	var TempUser Common.User

	err := ctx.BindJSON(&TempUser)
	if err != nil {
		log.Println("[error] attempting to read post body.", err)
		ctx.Status(http.StatusBadRequest)
		return
	}

	var success bool
	log.Println("[debug] ", TempUser)
	success, err = EndPoints.AuthenticateUser(TempUser.Username, TempUser.Password, DatabasePath)
	if success {
		ctx.Status(http.StatusOK)
		return
	}
	ctx.Status(http.StatusUnauthorized)
	return
}

func (t *TS) AddAuthenticateUserEndpoint() bool {

	DatabasePath = t.Server.FI.DataBasePath
	AuthUser := &Common.Endpoint{
		Endpoint: "AuthenticateUser",
		Function: t.AuthenticateUser,
	}

	return t.AddEndPoint(AuthUser)
}

func (t *TS) AddDefaultEndPoints() bool {
	t.AddAuthenticateUserEndpoint()
	return true
}
