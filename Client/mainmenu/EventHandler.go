package mainmenu

import (
	"Client/Common"
	"encoding/json"
	"github.com/gorilla/websocket"
	"log"
)

func CheckForNewEventsFromWS() {

	for {
		var NewWSMessage Common.WebSocketMessage
		err := g_clientobj.Conn.ReadJSON(&NewWSMessage)
		if err != nil {
			if websocket.IsUnexpectedCloseError(err, websocket.CloseGoingAway, websocket.CloseAbnormalClosure) {
				log.Printf("WebSocket closed unexpectedly: %v", err)
				break
			} else if websocket.IsCloseError(err, websocket.CloseNormalClosure, websocket.CloseGoingAway) {
				log.Printf("WebSocket closed normally: %v", err)
				break
			} else {
				log.Printf("[error] reading WebSocket message: %v", err)
				break
			}
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
	} // our websocket has closed. Attempt to reconnect.
	err := g_clientobj.Conn.Close() // since we can no longer read messages close our client and try to open a new one.
	if err != nil {
		log.Println("[error] attempting to close websocket connection", err)
	}
	g_clientobj.Authenticated = false
}
