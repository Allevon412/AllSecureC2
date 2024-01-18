package EndPoints

import (
	"AllSecure/TeamServer/Common"
	"database/sql"
	"encoding/json"
	"log"
)

func GetUserData(databasepath string) ([]byte, error) {
	var (
		db            *sql.DB
		err           error
		Rows          *sql.Rows
		TempUser      Common.DBUser
		TempJsonData  []byte
		FinalJsonData []byte
	)

	db, err = sql.Open("sqlite3", databasepath)
	if err != nil {
		log.Println("[error] Failed to open database", err)
		return []byte{}, err
	}

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
		//scan data into tempuser structure
		err = Rows.Scan(&TempUser.ID, &TempUser.Username, &TempUser.Admin)
		if err != nil {
			return []byte{}, err
		}

		TempJsonData, err = json.Marshal(TempUser)
		if err != nil {
			return []byte{}, err
		}
		FinalJsonData = append(FinalJsonData, TempJsonData...)
	}

	return FinalJsonData, nil
}
