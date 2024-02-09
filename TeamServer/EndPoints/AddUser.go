package EndPoints

import (
	"AllSecure/TeamServer/Common"
	"log"
)

// TODO create AddUser endpoint
func AddUser(NewUser Common.NewUser, DatabasePath string) {
	log.Println("NewUser Data!")
	log.Println(NewUser.Username)
	log.Println(NewUser.Password)
	log.Println(NewUser.Admin)
}
