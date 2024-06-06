package EndPoints

import (
	"AllSecure/TeamServer/Common/SQL"
	"AllSecure/TeamServer/Common/Types"
	"database/sql"
	"errors"
	"log"
)

func DeleteUser(NewUser Types.NewUser, DatabasePath string) error {
	var (
		db           *sql.DB
		err          error
		Stmt         *sql.Stmt
		result       sql.Result
		RowsAffected int64
		Exists       bool
	)
	Exists, err = SQL.CheckIfUserExists(NewUser.Username, DatabasePath)
	if err != nil {
		log.Println("[error] attempting to check if user exists in db. ", err)
		return err
	}
	if !Exists {
		return errors.New("user does not exist")
	}

	db, err = sql.Open("sqlite3", DatabasePath)
	if err != nil {
		log.Println("[error] Failed to open database", err)
		return err
	}
	defer db.Close()

	AddUserQuery := `
DELETE FROM users WHERE username = ?
`

	Stmt, err = db.Prepare(AddUserQuery)
	if err != nil {
		log.Println("[error] attempting query", err)
		return err
	}

	result, err = Stmt.Exec(NewUser.Username)
	if err != nil {
		return err
	}

	RowsAffected, err = result.RowsAffected()
	if err != nil {
		return err
	}

	if RowsAffected == 1 {
		return nil
	}
	return errors.New("invalid number of rows affected")
}
