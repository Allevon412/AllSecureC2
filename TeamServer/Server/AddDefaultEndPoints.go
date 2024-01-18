package Server

import (
	"AllSecure/TeamServer/Common"
	"AllSecure/TeamServer/EndPoints"
	"github.com/gin-gonic/gin"
	"log"
	"net/http"
)

// func to handle requests to the AuthenticateUser endpoint
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
	Admin, UserId, success, err = EndPoints.AuthenticateUser(TempUser.Username, TempUser.Password, t.Server.FI.DataBasePath)
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

// func to add the AuthenticateUser endpoint to the default list.
func (t *TS) AddAuthenticateUserEndpoint() bool {

	AuthUser := &Common.Endpoint{
		Endpoint: "AuthenticateUser",
		Function: t.AuthenticateUser,
	}

	return t.AddEndPoint(AuthUser)
}

// GetUserData endpoint request handler
func (t *TS) GetUserData(ctx *gin.Context) {
	var (
		claims   *Common.JWTClaims
		token    string
		err      error
		JsonData []byte
	)

	token = ctx.GetHeader("Authorization")
	if token == "" {
		ctx.JSON(http.StatusUnauthorized, gin.H{"error": "Invalid Parameters"})
		return
	}

	claims, err = t.ParseToken(token)
	if err != nil {
		ctx.JSON(http.StatusUnauthorized, gin.H{"error": "Invalid Parameters"})
		return
	}
	if !claims.Administrator {
		ctx.JSON(http.StatusUnauthorized, gin.H{"error": "Insufficient permissions"})
		return
	}

	JsonData, err = EndPoints.GetUserData(t.Server.FI.DataBasePath)
	if err != nil {
		log.Println("[error] attempting to retrieve user data from sql database", err)
		ctx.JSON(http.StatusInternalServerError, gin.H{"error": "Server error"})
		return
	}

	ctx.JSON(http.StatusOK, JsonData)
	return
}

// func to add GetUserData endpoint to the default list.
func (t *TS) AddGetUserDataEndpoint() bool {

	GetUserData := &Common.Endpoint{
		Endpoint: "GetUserData",
		Function: t.GetUserData,
	}
	return t.AddEndPoint(GetUserData)
}

// func to add all default endpoints to default list.
func (t *TS) AddDefaultEndPoints() bool {
	t.AddAuthenticateUserEndpoint()
	t.AddGetUserDataEndpoint()
	return true
}
