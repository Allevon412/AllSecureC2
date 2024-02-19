package mainmenu

import (
	"Client/Common"
	"encoding/json"
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/widget"
	"log"
	"strings"
)

func CreateListenerFunc() error {
	return nil
}
func StopListenerFunc() error {
	return nil
}
func GetListenerData(clientobj *Common.Client) ([]byte, error) {

	endpoint := "https://" + clientobj.Server + "/GetActiveListeners"
	data, err := Common.PerformHTTPReq(clientobj, endpoint, nil)
	if err != nil {
		log.Println("[error] attempting to retrieve user data", err)
	}
	trimedstr := strings.Trim(string(data), "\"")

	decoded := Common.Base64Decode(trimedstr)

	return decoded, nil
}

func CreateListenerTable(clientobj *Common.Client, window fyne.Window) {
	var (
		err error
	)
	data, err := GetListenerData(clientobj)
	if err != nil {
		log.Println("[error] attempting to obtain user data", err)
	}
	//unmarshal the data and store it.
	err = json.Unmarshal(Common.RemoveNullBytes(data), &Common.ListenerTableEntries)
	if err != nil {
		log.Println("[error] attempting to unmarshal data", err)
	}

	Create := fyne.NewMenuItem("Create Listener", func() {
		//create new user form when Add new user is selected from menu
		err = CreateListenerFunc()
		if err != nil {
			log.Println("[error] attempting to add user", err)
		}
	})

	//delete user popup menu
	Stop := fyne.NewMenuItem("Stop Listener", func() {
		err = StopListenerFunc()
		if err != nil {
			log.Println("[error] attempting to add user", err)
		}
	})
	//create a new menu from menu items
	menu := fyne.NewMenu("Options", Create, Stop)
	//create popupmenu from menu.
	PopUpMenu := widget.NewPopUpMenu(menu, window.Canvas())
	Common.ListenerTable = Common.CreateListenerTableObject(PopUpMenu)
}
