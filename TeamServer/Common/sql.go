package Common

import (
	"database/sql"
	"log"
)

func CheckIfUserExists(username, databasepath string) (bool, error) {
	var (
		db       *sql.DB
		err      error
		UserRow  *sql.Rows
		TempUser DBUser
	)

	db, err = sql.Open("sqlite3", databasepath)
	if err != nil {
		log.Println("[error] Failed to open database", err)
		return false, err
	}
	defer db.Close()

	GetUserQuery := `
SELECT username FROM users WHERE username = ?;
`
	UserRow, err = db.Query(GetUserQuery, username)

	if err != nil {
		log.Println("[error] attempting query", err)
		return false, err
	}
	defer UserRow.Close()

	for UserRow.Next() {
		err = UserRow.Scan(&TempUser.Username)
		if err != nil {
			return false, err
		}
	}

	if TempUser.Username != "" {
		return true, nil
	}
	return false, nil
}
