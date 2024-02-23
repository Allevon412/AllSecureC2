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

func AddListenerToSQLTable(username, databasepath string, userid int, data ListenerData) error {
	var (
		db   *sql.DB
		err  error
		stmt *sql.Stmt
	)
	db, err = sql.Open("sqlite3", databasepath)
	if err != nil {
		log.Println("[error] Failed to open database", err)
		return err
	}
	defer db.Close()
	InsertListenerIntoTableQuery := `
INSERT INTO listeners (user_id, listener_name, protocol, host, port_bind) VALUES (?, ?, ?, ?, ?)`
	stmt, err = db.Prepare(InsertListenerIntoTableQuery)
	if err != nil {
		return err
	}
	defer stmt.Close()
	data.UserID = userid
	data.UserName = username

	_, err = stmt.Exec(data.UserID, data.ListenerName, data.Protocol, data.HOST, data.PortBind)
	if err != nil {
		return err
	}

	return nil
}
