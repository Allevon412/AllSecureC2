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
		//lock the mutex to ensure race condition does not occur.
		lock.Lock()
		//overwrite previous the current chat log text.
		g_prevtext = teamsChatLog.Text
		//unlock mutex so other threads may use it.
		lock.Unlock()
		var NewMessage Common.WebSocketMessage
		//read message from server
		//cannot wrap whole funciton in mutex lock / unlcok since this blocks the thread it will never unlock until a new message is received.
		err = g_clientobj.Conn.ReadJSON(&NewMessage)
		if err != nil {
			log.Println("[error] attempting to read web socket message")
			return
		}
		//set teams chat to the previous text + the new message.
		lock.Lock()
		teamsChatLog.SetText(fmt.Sprintf("%s\n%s", g_prevtext, NewMessage.Message))
		lock.Unlock()
	}

}

func ClearChatHistory() {
	lock.Lock()
	teamsChatLog.SetText("")
	g_prevtext = ""
	lock.Unlock()
}
