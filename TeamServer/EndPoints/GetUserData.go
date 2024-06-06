package EndPoints

import (
	"AllSecure/TeamServer/Common/Types"
	"database/sql"
	"encoding/json"
	"log"
)

func GetUserData(databasepath string) ([]byte, error) {
	var (
		db            *sql.DB
		err           error
		Rows          *sql.Rows
		DBUserEntries []Types.DBUser
		FinalJsonData []byte
	)

	db, err = sql.Open("sqlite3", databasepath)
	if err != nil {
		log.Println("[error] Failed to open database", err)
		return []byte{}, err
	}
	defer db.Close()

	GetUserQuery := `
SELECT id, username, adminperms FROM users;
`
	Rows, err = db.Query(GetUserQuery)
	if err != nil {
		log.Println("[error] attempting query", err)
		return []byte{}, err
	}
	defer Rows.Close()
	for Rows.Next() { //loop over each user
		var TempUser Types.DBUser
		//scan data into tempuser structure
		err = Rows.Scan(&TempUser.ID, &TempUser.Username, &TempUser.Admin)
		if err != nil {
			return []byte{}, err
		}

		DBUserEntries = append(DBUserEntries, TempUser)
	}
	FinalJsonData, err = json.Marshal(DBUserEntries)
	if err != nil {
		return []byte{}, err
	}

	return FinalJsonData, nil
}
