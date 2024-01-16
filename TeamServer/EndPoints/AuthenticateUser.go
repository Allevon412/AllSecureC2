package EndPoints

import (
	"AllSecure/TeamServer/Common"
	"database/sql"
	_ "github.com/mattn/go-sqlite3"
	"golang.org/x/crypto/bcrypt"
	"log"
)

func AuthenticateUser(username, password, databasepath string) (bool, error) {
	var (
		db       *sql.DB
		err      error
		UserRow  *sql.Rows
		TempUser Common.DBUser
	)

	db, err = sql.Open("sqlite3", databasepath)
	if err != nil {
		log.Println("[error] Failed to open database", err)
		return false, err
	}

	if err != nil {
		log.Println("[error] hashing password", err)
		return false, err
	}

	GetUserQuery := `
SELECT id, username, password FROM users WHERE username = ?;
`
	UserRow, err = db.Query(GetUserQuery, username)

	if err != nil {
		log.Println("[error] attempting query", err)
		return false, err
	}
	defer UserRow.Close()

	UserRow.Next()
	err = UserRow.Scan(&TempUser.ID, &TempUser.Username, &TempUser.Password)
	if err != nil {
		return false, err
	}

	err = bcrypt.CompareHashAndPassword([]byte(TempUser.Password), []byte(password))
	if err == nil {
		return true, nil
	}
	return false, nil

}
