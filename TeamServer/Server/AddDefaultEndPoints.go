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
	var (
		TempUser Common.User
		UserId   int
		Admin    int
		token    string
	)

	err := ctx.BindJSON(&TempUser)
	if err != nil {
		log.Println("[error] attempting to read post body.", err)
		ctx.Status(http.StatusBadRequest)
		return
	}

	var success bool
	log.Println("[debug] ", TempUser)
	Admin, UserId, success, err = EndPoints.AuthenticateUser(TempUser.Username, TempUser.Password, DatabasePath)
	if success && UserId != 0 {
		//create the user's new jwt.
		token, err = t.CreateToken(UserId, TempUser.Username, Admin != 0) //Admin != 0 is checking if the integer is not equal to zero. In the case that it is, it will return false. In the case that it isn't it will return true.
		if err != nil {
			log.Println("[error] attempting to create jwt token", token, err)
		}
		ctx.JSON(http.StatusOK, gin.H{"token": token})
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
