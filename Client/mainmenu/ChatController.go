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
		Message:     fmt.Sprintf("%s [%s] :: %s", date.Format("2006-01-02 15:04:05"), TeamsChatLog.ImplantName, EnteredText),
	}

	err := g_clientobj.Conn.WriteJSON(ChatMessage)
	if err != nil {
		log.Println("[error] attempting to write chat message to the server websocket.", err)
		return
	}
	TeamsChatLog.EntryBar.SetText("")
	return
}

func UpdateChat(Message string) {

	//set teams chat to the previous text + the new message0.
	lock.Lock()
	TeamsChatLog.Text = append(TeamsChatLog.Text, fmt.Sprintf("%s", Message))
	TeamsChatLog.ImplantLog.Refresh()
	TeamsChatLog.ImplantLog.ScrollToBottom()
	lock.Unlock()

}

func ClearChatHistory() {
	lock.Lock()
	TeamsChatLog.Text = []string{}
	lock.Unlock()
}
