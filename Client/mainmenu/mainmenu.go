package mainmenu

import (
	"Client/Common"
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/driver/desktop"
	"fyne.io/fyne/v2/layout"
	"fyne.io/fyne/v2/widget"
	"log"
	"sync"
)

var (
	TeamsChatLog *Common.ImplantInteractionMenu
	chaticon     fyne.Resource
	g_clientobj  *Common.Client
	lock         sync.Mutex
)

func CreateMenuItems(clientobj *Common.Client, OldWindow fyne.App, TeamsChat *fyne.Container, tabs *container.DocTabs) *fyne.MainMenu {

	Common.ListenerTable.Refresh()
	mainMenu := fyne.NewMainMenu(
		fyne.NewMenu("Options",
			fyne.NewMenuItem("View Users", func() { CreateUserWindow(clientobj, OldWindow) }),
			fyne.NewMenuItem("Add User", func() { AddUser(clientobj, OldWindow) }),
			//fyne.NewMenuItem("Delete User", func() { RemoveUser(clientobj, OldWindow) }),
			fyne.NewMenuItem("Create Listener", func() { CreateListenerFunc(clientobj, OldWindow) }),
			//fyne.NewMenuItem("Stop Listener", func() { StopListenerFunc(clientobj, OldWindow) }),
		),
		fyne.NewMenu("View",
			fyne.NewMenuItem("Team Chat", func() { AddTab(tabs, TeamsChat, "Teams Chat") }),
			fyne.NewMenuItem("Listeners", func() { AddTab(tabs, Common.ListenerTable, "Active Listeners") }),
			fyne.NewMenuItem("Logging", func() { AddTab(tabs, widget.NewLabel("Logging Test"), "Logs") }),
		),
		fyne.NewMenu("Payload Builder",
			fyne.NewMenuItem("Build Payload", func() { ImplantBuilderForm(clientobj, OldWindow) }),
		),
		fyne.NewMenu("Help",
			fyne.NewMenuItem("Github", func() {}),
			fyne.NewMenuItem("Documentation", func() {}),
		),
	)
	return mainMenu
}

// TODO figure out how to make this bigger so it looks like an actual chat window.
func CreateChatForm(Username string) (*fyne.Container, error) {
	//create websocket channel for chat.
	go ObtainWebSocketConn(g_clientobj)

	TeamsChatLog = &Common.ImplantInteractionMenu{
		Text: []string{},
		ImplantLog: Common.NewExtendedList(10, func() int {
			return len(TeamsChatLog.Text)
		},
			func() fyne.CanvasObject {
				return container.New(layout.CustomPaddedLayout{
					TopPadding:    -7.5,
					BottomPadding: -7.5,
					LeftPadding:   0,
					RightPadding:  0,
				}, Common.NewCustomLabelWidget(func(m *desktop.MouseEvent) {}))
			},
			func(i widget.ListItemID, o fyne.CanvasObject) {
				o.(*fyne.Container).Objects[0].(*Common.CustomLabelWidget).SetText(TeamsChatLog.Text[i])
			}),
		EntryBar:    Common.NewCustomChatEntry(SendChat),
		ImplantName: Username,
	}

	TeamsChatLog.ImplantLog.HideSeparators = true
	//Teams chat entry's combined into one form.
	TeamsChat := &widget.Form{
		Items: []*widget.FormItem{
			{Text: Username, Widget: TeamsChatLog.EntryBar},
		},
		OnSubmit: func() {
			SendChat(TeamsChatLog.EntryBar.Text)
			TeamsChatLog.EntryBar.SetText("")
		},
		OnCancel:   ClearChatHistory,
		SubmitText: "",
		CancelText: "Clear Chat History",
	}

	border := container.NewBorder(
		TeamsChatLog.ImplantLog, TeamsChat, nil, nil, nil)

	return border, nil
}

func ObtainWebSocketConn(clientobj *Common.Client) error {
	var (
		err error
	)
	clientobj.Conn, err = Common.ObtainWebSocket(clientobj)
	if err != nil {
		clientobj.Authenticated = false
		log.Println("[error] attempting to obtain web socket connection.")
	}

	CheckForNewEventsFromWS()

	return nil
}

func MainMenu(clientobj *Common.Client, FyneApp fyne.App, icon fyne.Resource, ResourcePath string) {

	//Create new window
	NewWindow := FyneApp.NewWindow("AllSecure")

	g_clientobj = clientobj

	CreateListenerTable(clientobj, NewWindow, FyneApp)

	//Create new teams chat
	TeamsChat, err := CreateChatForm(clientobj.Username)
	if err != nil {
		log.Println("[error] attempting to create chat form. ", err)
	}

	//load chat icon
	chaticon, err = fyne.LoadResourceFromPath(ResourcePath + "chat.ico")
	if err != nil {
		log.Println("[error] attempting to load chat icon", err)
	}

	//create new document tab container
	tabs := container.NewDocTabs()

	//creating all menu items.
	mainMenu := CreateMenuItems(clientobj, FyneApp, TeamsChat, tabs)

	//create initial tab
	tabs.Append(container.NewTabItemWithIcon("Teams Chat", chaticon, TeamsChat))
	// Create content for table pane
	Common.ImplantTable = CreateImplantTable(NewWindow, FyneApp)

	// TODO create logging pane.
	topRightContent := widget.NewLabel("Logging Pane")

	// Create a horizontal split for the top panes
	topsplit := container.NewHSplit(Common.ImplantTable, topRightContent)

	// Create a border layout with the top and bottom panes
	TopCenteredSplitContent := container.NewBorder(
		nil, tabs, // No left or right panes
		nil, nil, // Top and bottom panes
		topsplit, // Center pane (topSplit)
	)

	//set window size parameters + main menu + the 3 window panes + icon.
	NewWindow.SetMainMenu(mainMenu)
	NewWindow.SetContent(TopCenteredSplitContent)
	NewWindow.Resize(fyne.NewSize(1920, 1080))
	NewWindow.SetIcon(icon)

	//show content. it's already running.
	NewWindow.Show()

	//Get Any Active Implants on Startup
	ImplantMessage := Common.WebSocketMessage{
		MessageType: "GetActiveImplants",
		Message:     "",
	}
	_, err = GetActiveImplants(clientobj, ImplantMessage)
	if err != nil {
		log.Println("[error] attempting to obtain active implants", err)
	}
	Common.ImplantTable.Refresh()

	NewWindow.SetCloseIntercept(func() {
		clientobj.Conn.Close()
		NewWindow.Close()
	})

}
