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
	    perms VARCHAR(16) NULL,
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
func (config *AllSecureConfig) InsertUserIntoTable(db *sql.Tx, username string, hashedpassword string) bool {
	var err error
	InsertUser := `
	INSERT INTO users (username, password) VALUES (?, ?);
`
	_, err = db.Exec(InsertUser, config.Username, hashedpassword)
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
