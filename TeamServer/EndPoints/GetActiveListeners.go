package EndPoints

import (
	"AllSecure/TeamServer/Common"
	"database/sql"
	"encoding/json"
	"log"
)

func GetActiveListeners(Listeners []Common.ListenerData, databasepath string, claims *Common.JWTClaims) ([]byte, error) {
	var (
		db                 *sql.DB
		err                error
		Rows               *sql.Rows
		DBListenersEntries []Common.ListenerData
		FinalJsonData      []byte
	)

	db, err = sql.Open("sqlite3", databasepath)
	if err != nil {
		log.Println("[error] Failed to open database", err)
		return []byte{}, err
	}
	defer db.Close()

	GetListenersByUserQuery := `
SELECT ld.listener_name,  u.username, ld.port_conn, ld.host, ld.protocol, ld.port_bind FROM users u JOIN listeners ld ON u.id = ld.user_id;
`
	Rows, err = db.Query(GetListenersByUserQuery)
	if err != nil {
		log.Println("[error] attempting query", err)
		return []byte{}, err
	}

	defer Rows.Close()
	for Rows.Next() { //loop over each user
		var TempListener Common.ListenerData
		//scan data into tempuser structure
		err = Rows.Scan(&TempListener.ListenerName, &TempListener.UserName, &TempListener.PortConn, &TempListener.HOST, &TempListener.Protocol, &TempListener.PortBind)
		if err != nil {
			return []byte{}, err
		}

		DBListenersEntries = append(DBListenersEntries, TempListener)
	}

	FinalJsonData, err = json.Marshal(DBListenersEntries)
	if err != nil {
		return []byte{}, err
	}

	return FinalJsonData, nil
}