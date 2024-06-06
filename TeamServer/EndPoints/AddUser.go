package EndPoints

import (
	"AllSecure/TeamServer/Common/SQL"
	"AllSecure/TeamServer/Common/Types"
	"database/sql"
	"errors"
	"golang.org/x/crypto/bcrypt"
	"log"
)

func AddUser(NewUser Types.NewUser, DatabasePath string) error {

	var (
		db             *sql.DB
		err            error
		Stmt           *sql.Stmt
		HashedPassword []byte
		result         sql.Result
		RowsAffected   int64
		Exists         bool
	)
	Exists, err = SQL.CheckIfUserExists(NewUser.Username, DatabasePath)
	if err != nil {
		log.Println("[error] attempting to check if user exists in db. ", err)
		return err
	}
	if Exists {
		return errors.New("user with username already exists")
	}

	db, err = sql.Open("sqlite3", DatabasePath)
	if err != nil {
		log.Println("[error] Failed to open database", err)
		return err
	}
	defer db.Close()

	AddUserQuery := `
INSERT INTO users(username, password, adminperms) VALUES(?, ?, ?);
`

	Stmt, err = db.Prepare(AddUserQuery)
	if err != nil {
		log.Println("[error] attempting query", err)
		return err
	}
	HashedPassword, err = bcrypt.GenerateFromPassword([]byte(NewUser.Password), bcrypt.DefaultCost)
	if err != nil {
		return err
	}
	if NewUser.Admin == true {
		result, err = Stmt.Exec(NewUser.Username, string(HashedPassword), 1)
	} else {
		result, err = Stmt.Exec(NewUser.Username, string(HashedPassword), 0)
	}
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
