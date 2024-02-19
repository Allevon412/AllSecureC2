package Config

import (
	"database/sql"
	_ "github.com/mattn/go-sqlite3"
	"golang.org/x/crypto/bcrypt"
	"log"
)

func HashPassword(password string) (string, error) {
	HashedPassword, err := bcrypt.GenerateFromPassword([]byte(password), bcrypt.DefaultCost)
	if err != nil {
		return "", err
	}
	return string(HashedPassword), nil
}

func (config *AllSecureConfig) CreateUserTable(db *sql.Tx) bool {
	var err error

	CreateUserTable := `
	CREATE TABLE IF NOT EXISTS users (
	    id INTEGER PRIMARY KEY AUTOINCREMENT,
	    adminperms INTEGER NOT NULL,
	    username TEXT NOT NULL,
	    password TEXT NOT NULL
	);
`
	_, err = db.Exec(CreateUserTable)
	if err != nil {
		log.Fatalln("Error creating user table", err)
		return false
	}

	return true
}

func (config *AllSecureConfig) CreateListenersTable(db *sql.Tx) bool {
	var err error
	CreateListenersTable := `
CREATE TABLE IF NOT EXISTS listeners (
    id INTEGER PRIMARY KEY,
    user_id INTEGER,
    listener_name TEXT,
    protocol TEXT,
    host TEXT,
    port_bind INTEGER,
    port_conn TEXT,
    FOREIGN KEY (user_id) REFERENCES users(id)
    );`

	_, err = db.Exec(CreateListenersTable)
	if err != nil {
		log.Fatalln("[Error] attempting to create implant table with user_id association", err)
		return false
	}

	return true
}

func (config *AllSecureConfig) CreateImplantsTable(db *sql.Tx) bool {
	var err error
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

	_, err = db.Exec(CreateImplantTable)
	if err != nil {
		log.Fatalln("[Error] attempting to create implant table with user_id association", err)
		return false
	}

	return true
}

func (config *AllSecureConfig) InsertUserIntoTable(db *sql.Tx, username string, hashedpassword string) bool {
	var err error
	InsertUser := `
	INSERT INTO users (username, password, adminperms) VALUES (?, ?, ?);
`
	_, err = db.Exec(InsertUser, config.Username, hashedpassword, 1)
	if err != nil {
		log.Fatalln("[!] error inserting user into table", err)
		return false
	}
	return true
}

func (config *AllSecureConfig) InitDatabase() {
	var (
		db             *sql.DB
		tx             *sql.Tx
		err            error
		HashedPassword string
	)

	db, err = sql.Open("sqlite3", config.DatabaseFullPath)
	if err != nil {
		log.Fatalln(err)
	}

	defer db.Close()

	tx, err = db.Begin()
	if err != nil {
		log.Fatalln("[error] Error beginning transaction", err)
	}

	//Create User Table
	if !config.CreateUserTable(tx) {
		tx.Rollback()
		log.Fatalln("[error] Error inserting first user")
	}
	//Create Listeners Table
	if !config.CreateListenersTable(tx) {
		tx.Rollback()
		log.Fatalln("[error] creating listeners table.")
	}
	//Create Implants Table
	if !config.CreateImplantsTable(tx) {
		tx.Rollback()
		log.Fatalln("[error] creating implants table.")
	}

	//Insert User Into Table
	HashedPassword, err = HashPassword(config.Password)
	if err != nil {
		log.Fatalln("[!] Error generating password hash!", err)
	}
	if !config.InsertUserIntoTable(tx, config.Username, HashedPassword) {
		tx.Rollback()
		log.Fatalln("[error] Error inserting first user")
	}

	// Commit the transaction
	err = tx.Commit()
	if err != nil {
		log.Fatalln("Error committing transaction:", err)
	}

}
