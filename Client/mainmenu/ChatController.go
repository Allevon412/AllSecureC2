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
		log.Println("[error] attempting to write chat message to the server websocket.", err)
		return
	}
	customEntryWidget.SetText("")
	return
}

// TODO: change the number of maximum messages shown. delete the last one. (maybe use a linkedlist?)
func UpdateChat(Message string) {

	//lock the mutex to ensure race condition does not occur.
	lock.Lock()
	//overwrite previous the current chat log text.
	g_prevtext = teamsChatLog.Text
	//unlock mutex so other threads may use it.
	lock.Unlock()
	//set teams chat to the previous text + the new message0.
	lock.Lock()
	teamsChatLog.SetText(fmt.Sprintf("%s\n%s", g_prevtext, Message))
	length := len(teamsChatLog.Text)
	teamsChatLog.CursorRow = length
	teamsChatLog.Refresh()
	lock.Unlock()

}

func ClearChatHistory() {
	lock.Lock()
	teamsChatLog.SetText("")
	g_prevtext = ""
	lock.Unlock()
}
