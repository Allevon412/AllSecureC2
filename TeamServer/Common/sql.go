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

func GetListenerData(databasepath string) ([]ListenerData, error) {
	var (
		db          *sql.DB
		err         error
		ListenerRow *sql.Rows
		listdata    ListenerData
	)

	db, err = sql.Open("sqlite3", databasepath)
	if err != nil {
		log.Println("[error] Failed to open database", err)
		return []ListenerData{}, err
	}
	defer db.Close()

	Query := `
SELECT u.username, ld.user_id, ld.listener_name, ld.protocol, ld.host, ld.port_bind FROM users u JOIN listeners ld ON u.id = ld.user_id`

	ListenerRow, err = db.Query(Query)
	if err != nil {
		log.Println("[error] failed to retrieve database rows", err)
		return []ListenerData{}, err
	}
	var data_slice []ListenerData
	for ListenerRow.Next() {
		err = ListenerRow.Scan(&listdata.UserName, &listdata.UserID, &listdata.ListenerName, &listdata.Protocol, &listdata.HOST, &listdata.PortBind)
		if err != nil {
			return []ListenerData{}, err
		}
		data_slice = append(data_slice, listdata)
	}
	return data_slice, nil
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

func RemoveListenerFromSQLTable(databasepath string, data ListenerData) (ListenerData, error) {
	var (
		db          *sql.DB
		err         error
		stmt        *sql.Stmt
		ListenerRow *sql.Rows
		listdata    ListenerData
	)
	db, err = sql.Open("sqlite3", databasepath)
	if err != nil {
		log.Println("[error] Failed to open database", err)
		return ListenerData{}, err
	}
	defer db.Close()
	SelectListenerToReturnQuery := `
SELECT listener_name, protocol, host, port_bind FROM listeners WHERE listener_name = ?`
	ListenerRow, err = db.Query(SelectListenerToReturnQuery, data.ListenerName)
	if err != nil {
		return ListenerData{}, err
	}
	defer ListenerRow.Close()

	for ListenerRow.Next() {
		err = ListenerRow.Scan(&listdata.ListenerName, &listdata.Protocol, &listdata.HOST, &listdata.PortBind)
		if err != nil {
			return ListenerData{}, err
		}
	}

	InsertListenerIntoTableQuery := `
DELETE FROM listeners WHERE listener_name = ?`

	stmt, err = db.Prepare(InsertListenerIntoTableQuery)
	if err != nil {
		return ListenerData{}, err
	}
	defer stmt.Close()

	_, err = stmt.Exec(data.ListenerName)
	if err != nil {
		return ListenerData{}, err
	}

	return listdata, nil
}

/*
	CreateImplantTable := `
CREATE TABLE IF NOT EXISTS implants (
    id INTEGER PRIMARY KEY,
    user_id INTEGER,
    implant_id TEXT,
    external_ip TEXT,
    internal_ip TEXT,
    username TEXT,
    computer_name TEXT,
    pid INTEGER,
    process_name TEXT,
    health TEXT,
    last_checkin TEXT,
    FOREIGN KEY (user_id) REFERENCES users(id)
    );`
*/

/*
func AddImplantToSqlTable(databasepath string, data ImplantData) error {
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
	InsertImplantIntoTableQuery := `
INSERT INTO implants (user_id, implant_id, external_ip, internal_ip, username, computer_name, pid, process_name, health, last_checkin) VALUES (?, ?, ?, ?, ?, ?, ?, ?)`
}

*/
