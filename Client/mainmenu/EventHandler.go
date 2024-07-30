package mainmenu

import (
	"Client/Common"
	"encoding/json"
	"log"
)

// TODO ensure that the process does not crash if this is unable to connect to remote host
func CheckForNewEventsFromWS() {
	for {
		var NewWSMessage Common.WebSocketMessage
		err := g_clientobj.Conn.ReadJSON(&NewWSMessage)
		if err != nil {
			log.Println("[error] attempting to read web socket message ", err)
			continue
		}
		switch NewWSMessage.MessageType {
		case "ChatMessage":
			UpdateChat(NewWSMessage.Message)
			break
		case "RegisterImplant":
			var TempImplantData Common.ImplantTableData
			err = json.Unmarshal([]byte(NewWSMessage.Message), &TempImplantData)
			if err != nil {
				log.Println("[error] attempting to read implant data from web socket message", err)
				continue
			}

			AddImplantToTable(TempImplantData)
			break
		default:
			break
		}
	}
}
