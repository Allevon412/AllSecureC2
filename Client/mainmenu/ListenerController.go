package mainmenu

import (
	"Client/Common"
	"encoding/json"
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
	"log"
	"strconv"
	"strings"
)

func CreateListenerSubmitFunc(ListNameEntry, ListProtocol, ListHost, ListPort *Common.CustomEntry, clientobj *Common.Client, NewWindow fyne.Window) {
	var (
		Jdata        []byte
		err          error
		ListenerData Common.ListenerData
		data         []byte
	)
	//close the form window at end of the function.
	defer NewWindow.Close()

	ListenerData.ListenerName = ListNameEntry.Text
	ListenerData.HOST = ListHost.Text
	ListenerData.Protocol = ListProtocol.Text
	ListenerData.PortBind, err = strconv.Atoi(ListPort.Text)
	if err != nil {
		log.Println("[error] attempting to convert listener port to integer format", err)
		return
	}
	Jdata, err = json.Marshal(ListenerData)
	if err != nil {
		log.Println("[error] attempting to marshal listener data", err)
		return
	}
	ChatMessage := Common.WebSocketMessage{
		MessageType: "CreateListener",
		Message:     string(Jdata),
	}

	err = g_clientobj.Conn.WriteJSON(ChatMessage)
	if err != nil {
		log.Println("[error] attempting to write chat message to the server websocket.", err)
		return
	}

	data, err = GetListenerData(clientobj)
	if err != nil {
		log.Println("[error] attempting to obtain new listener data after adding new listener", err)
		return
	}
	err = json.Unmarshal(Common.RemoveNullBytes(data), &Common.ListenerTableEntries)
	if err != nil {
		log.Println("[error] attempting to unmarshal data", err)
	}
	//refresh the table with the newly added user.
	Common.ListenerTable.Refresh()
	//exit. listener added!
	return
}

func StopListenerSubmitFunc(ListNameEntry *Common.CustomEntry, clientobj *Common.Client, NewWindow fyne.Window) {
	// close the stop listener form window.
	defer NewWindow.Close()

	var (
		ListenerData Common.ListenerData
		err          error
		Jdata        []byte
		data         []byte
	)

	ListenerData.ListenerName = ListNameEntry.Text

	if err != nil {
		log.Println("[error] attempting to convert listener port to integer format", err)
		return
	}
	Jdata, err = json.Marshal(ListenerData)
	if err != nil {
		log.Println("[error] attempting to marshal listener data", err)
		return
	}
	ChatMessage := Common.WebSocketMessage{
		MessageType: "RemoveListener",
		Message:     string(Jdata),
	}

	err = g_clientobj.Conn.WriteJSON(ChatMessage)
	if err != nil {
		log.Println("[error] attempting to write chat message to the server websocket.", err)
		return
	}

	data, err = GetListenerData(clientobj)
	if err != nil {
		log.Println("[error] attempting to obtain new listener data after adding new listener", err)
		return
	}
	err = json.Unmarshal(Common.RemoveNullBytes(data), &Common.ListenerTableEntries)
	if err != nil {
		log.Println("[error] attempting to unmarshal data", err)
	}
	//refresh the table with the newly added user.
	Common.ListenerTable.Refresh()
	//exit. listener added!
	return
}

func CreateListenerFunc(clientobj *Common.Client, OldWindow fyne.App) error {
	NewWindow := OldWindow.NewWindow("Create Listener Form")

	var (
		ListNameEntry, ListProtocol, ListHost, ListPort *Common.CustomEntry
	)
	ListNameEntry = Common.NewCustomCredentialEntry(func() {
		CreateListenerSubmitFunc(ListNameEntry, ListProtocol, ListHost, ListPort, clientobj, NewWindow)
	})
	ListProtocol = Common.NewCustomCredentialEntry(func() {
		CreateListenerSubmitFunc(ListNameEntry, ListProtocol, ListHost, ListPort, clientobj, NewWindow)
	})
	ListHost = Common.NewCustomCredentialEntry(func() {
		CreateListenerSubmitFunc(ListNameEntry, ListProtocol, ListHost, ListPort, clientobj, NewWindow)
	})
	ListPort = Common.NewCustomCredentialEntry(func() {
		CreateListenerSubmitFunc(ListNameEntry, ListProtocol, ListHost, ListPort, clientobj, NewWindow)
	})

	form := &widget.Form{
		BaseWidget: widget.BaseWidget{},
		Items: []*widget.FormItem{
			{
				Text:     "Listener Name",
				Widget:   ListNameEntry,
				HintText: "HTTPS_LISTENER",
			},
			{
				Text:     "Protocol",
				Widget:   ListProtocol,
				HintText: "HTTPS",
			},
			{
				Text:     "Serving Host",
				Widget:   ListHost,
				HintText: "127.0.0.1",
			},
			{
				Text:     "Listening Port",
				Widget:   ListPort,
				HintText: "443",
			},
		},
		OnSubmit: func() {
			CreateListenerSubmitFunc(ListNameEntry, ListProtocol, ListHost, ListPort, clientobj, NewWindow)
		},
		OnCancel:   NewWindow.Close,
		SubmitText: "Create Listener",
		CancelText: "Exit",
	}

	//show our form in the new window.
	stack := container.NewStack(form)
	NewWindow.SetContent(stack)
	NewWindow.Resize(fyne.NewSize(700, 300))
	NewWindow.Show()
	return nil
}
func StopListenerFunc(clientobj *Common.Client, OldWindow fyne.App) error {
	NewWindow := OldWindow.NewWindow("Stop Listener Form")

	var (
		ListNameEntry *Common.CustomEntry
	)

	ListNameEntry = Common.NewCustomCredentialEntry(func() { StopListenerSubmitFunc(ListNameEntry, clientobj, NewWindow) })
	form := &widget.Form{
		BaseWidget: widget.BaseWidget{},
		Items: []*widget.FormItem{
			{
				Text:     "Listener Name",
				Widget:   ListNameEntry,
				HintText: "HTTPS_LISTENER",
			},
		},
		OnSubmit: func() {
			StopListenerSubmitFunc(ListNameEntry, clientobj, NewWindow)
		},
		OnCancel:   NewWindow.Close,
		SubmitText: "Stop Listener",
		CancelText: "Exit",
	}

	//show our form in the new window.
	stack := container.NewStack(form)
	NewWindow.SetContent(stack)
	NewWindow.Resize(fyne.NewSize(700, 300))
	NewWindow.Show()
	return nil
}
func GetListenerData(clientobj *Common.Client) ([]byte, error) {

	endpoint := "https://" + clientobj.Server + "/GetActiveListeners"
	data, err := Common.PerformHTTPReq(clientobj, endpoint, nil)
	if err != nil {
		if err.Error() == "unauthorized" {
			clientobj.Authenticated = false
		}
		log.Println("[error] attempting to retrieve listener data", err)
	}

	trimedstr := strings.Trim(string(data), "\"")

	decoded := Common.Base64Decode(trimedstr)

	return decoded, nil
}

func CreateListenerTable(clientobj *Common.Client, window fyne.Window, OldWindow fyne.App) {
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
		err = CreateListenerFunc(clientobj, OldWindow)
		if err != nil {
			log.Println("[error] attempting to add user", err)
		}
	})

	//delete user popup menu
	Stop := fyne.NewMenuItem("Stop Listener", func() {
		err = StopListenerFunc(clientobj, OldWindow)
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
