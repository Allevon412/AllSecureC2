package mainmenu

import (
	"fmt"
	"time"
)

func UpdateChat(EnteredText string) {
	prevtext := teamsChatLog.Text
	date := time.Now()
	teamsChatLog.SetText(fmt.Sprintf("%s\n%s [%s] :: %s", prevtext, date.Format("2006-01-02 15:04:05"), username, EnteredText))
}
