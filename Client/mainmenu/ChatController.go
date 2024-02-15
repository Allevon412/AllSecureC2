package mainmenu

import (
	"Client/Common"
	"fmt"
	"log"
	"time"
)

func SendChat(EnteredText string) {

	date := time.Now()
	ChatMessage := Common.WebSocketMessage{
		MessageType: "ChatMessage",
		Message:     fmt.Sprintf("%s [%s] :: %s", date.Format("2006-01-02 15:04:05"), g_username, EnteredText),
	}

	err := g_clientobj.Conn.WriteJSON(ChatMessage)
	if err != nil {
		log.Println("[error] attempting to write test message to the server websocket.")
		return
	}

	return
}
func UpdateChat() {

	var err error
	for {
		lock.Lock()
		g_prevtext = teamsChatLog.Text
		lock.Unlock()
		var NewMessage Common.WebSocketMessage
		//g_clientobj.Conn.SetReadDeadline(time.Now().Add(time.Second * 1 / 2))
		err = g_clientobj.Conn.ReadJSON(&NewMessage)
		if err != nil {
			//	if websocket.IsUnexpectedCloseError(err, websocket.CloseGoingAway, websocket.CloseAbnormalClosure) {
			//		continue
			//	}
			log.Println("[error] attempting to read web socket message")
			return
		}
		teamsChatLog.SetText(fmt.Sprintf("%s\n%s", g_prevtext, NewMessage.Message))
	}

}

func ClearChatHistory() {
	teamsChatLog.SetText("")
	lock.Lock()
	g_prevtext = ""
	lock.Unlock()
}
