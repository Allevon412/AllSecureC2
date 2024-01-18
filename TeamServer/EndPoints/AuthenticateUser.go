package EndPoints

import (
	"AllSecure/TeamServer/Common"
	"database/sql"
	_ "github.com/mattn/go-sqlite3"
	"golang.org/x/crypto/bcrypt"
	"log"
)

func AuthenticateUser(username, password, databasepath string) (int, int, bool, error) {
	var (
		db       *sql.DB
		err      error
		UserRow  *sql.Rows
		TempUser Common.DBUser
	)

	db, err = sql.Open("sqlite3", databasepath)
	if err != nil {
		log.Println("[error] Failed to open database", err)
		return 0, 0, false, err
	}

	GetUserQuery := `
SELECT id, username, password, adminperms FROM users WHERE username = ?;
`
	UserRow, err = db.Query(GetUserQuery, username)

	if err != nil {
		log.Println("[error] attempting query", err)
		return 0, 0, false, err
	}
	defer UserRow.Close()

	UserRow.Next()
	err = UserRow.Scan(&TempUser.ID, &TempUser.Username, &TempUser.Password, &TempUser.Admin)
	if err != nil {
		return 0, 0, false, err
	}

	err = bcrypt.CompareHashAndPassword([]byte(TempUser.Password), []byte(password))
	if err == nil {
		return TempUser.Admin, TempUser.ID, true, nil
	}
	return 0, 0, false, nil

}
