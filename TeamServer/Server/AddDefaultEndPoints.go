package Server

import (
	"AllSecure/TeamServer/Common/Types"
	"AllSecure/TeamServer/EndPoints"
	"github.com/gin-gonic/gin"
	"log"
	"net/http"
)

// func to handle requests to the AuthenticateUser endpoint
func (t *TS) AuthenticateUser(ctx *gin.Context) {
	var (
		TempUser Types.User
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
	Admin, UserId, success, err = EndPoints.AuthenticateUser(TempUser.Username, TempUser.Password, t.Server.TSConfig.DatabasePath)
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

	AuthUser := &Types.Endpoint{
		Endpoint: "AuthenticateUser",
		Function: t.AuthenticateUser,
	}

	return t.AddEndPoint(AuthUser)
}

// GetUserData endpoint request handler
func (t *TS) GetUserData(ctx *gin.Context) {
	var (
		claims   *Types.JWTClaims
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

	JsonData, err = EndPoints.GetUserData(t.Server.TSConfig.DatabasePath)
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

	GetUserData := &Types.Endpoint{
		Endpoint: "GetUserData",
		Function: t.GetUserData,
	}
	return t.AddEndPoint(GetUserData)
}

func (t *TS) AddNewUser(ctx *gin.Context) {
	var (
		claims  *Types.JWTClaims
		NewUser Types.NewUser
		token   string
		err     error
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

	err = ctx.BindJSON(&NewUser)
	if err != nil {
		log.Println("[error] attempting to bind new user data", err)
		ctx.JSON(http.StatusInternalServerError, gin.H{"error": "Internal Server Error"})
		return
	}
	err = EndPoints.AddUser(NewUser, t.Server.TSConfig.DatabasePath)
	if err != nil {
		log.Println("[error] attempting to add user to database", err)
		ctx.JSON(http.StatusInternalServerError, gin.H{"error": "Internal Server Error"})
		return
	}
}

func (t *TS) AddAddNewUserEndPoint() bool {
	AddNewUser := &Types.Endpoint{
		Endpoint: "AddNewUser",
		Function: t.AddNewUser,
	}
	return t.AddEndPoint(AddNewUser)
}

func (t *TS) DeleteUser(ctx *gin.Context) {
	var (
		claims  *Types.JWTClaims
		NewUser Types.NewUser
		token   string
		err     error
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

	err = ctx.BindJSON(&NewUser)
	if err != nil {
		log.Println("[error] attempting to bind new user data", err)
		ctx.JSON(http.StatusInternalServerError, gin.H{"error": "Internal Server Error"})
		return
	}
	err = EndPoints.DeleteUser(NewUser, t.Server.TSConfig.DatabasePath)
	if err != nil {
		log.Println("[error] attempting to add user to database", err)
		ctx.JSON(http.StatusInternalServerError, gin.H{"error": "Internal Server Error"})
		return
	}
}

func (t *TS) AddDeleteUserEndPoint() bool {
	DeleteUser := &Types.Endpoint{
		Endpoint: "DeleteUser",
		Function: t.DeleteUser,
	}
	return t.AddEndPoint(DeleteUser)
}

func (t *TS) GetActiveListeners(ctx *gin.Context) {
	var (
		claims    *Types.JWTClaims
		Listeners []Types.ListenerData
		JsonData  []byte
		token     string
		err       error
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
	if claims.UserID <= 0 || claims.Username == "" {
		ctx.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Parameters"})
		return
	}
	JsonData, err = EndPoints.GetActiveListeners(Listeners, t.Server.TSConfig.DatabasePath, claims)
	if err != nil {
		log.Println("[error] attempting to retrieve user data from sql database", err)
		ctx.JSON(http.StatusInternalServerError, gin.H{"error": "Server error"})
		return
	}

	ctx.JSON(http.StatusOK, JsonData)
	return
}

func (t *TS) AddGetActiveListenersEndPoint() bool {
	GetActiveListenersEndpoint := &Types.Endpoint{
		Endpoint: "GetActiveListeners",
		Function: t.GetActiveListeners,
	}
	return t.AddEndPoint(GetActiveListenersEndpoint)
}

func (t *TS) CreateImplant(ctx *gin.Context) {
	var (
		claims *Types.JWTClaims
		token  string
		err    error
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
	if claims.UserID <= 0 || claims.Username == "" {
		ctx.JSON(http.StatusBadRequest, gin.H{"error": "Invalid Parameters"})
		return
	}

	EndPoints.CreateImplant()

	ctx.JSON(http.StatusOK, gin.H{"message": "Implant Created"})
	return
}

func (t *TS) AddCreateImplantEndPoint() bool {
	CreateImplant := &Types.Endpoint{
		Endpoint: "CreateImplant",
		Function: t.CreateImplant,
	}
	return t.AddEndPoint(CreateImplant)

}

// func to add all default endpoints to default list.
func (t *TS) AddDefaultEndPoints() bool {
	t.AddAuthenticateUserEndpoint()
	t.AddGetUserDataEndpoint()
	t.AddAddNewUserEndPoint()
	t.AddDeleteUserEndPoint()
	t.AddGetActiveListenersEndPoint()
	return true
}
