package mainmenu

import (
	"Client/Common"
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
	"log"
	"sync"
)

var (
	teamsChatLog      *widget.Entry
	customEntryWidget *Common.CustomChatEntry
	g_username        string
	chaticon          fyne.Resource
	g_clientobj       *Common.Client
	g_prevtext        string
	lock              sync.Mutex
)

func CreateMenuItems(clientobj *Common.Client, OldWindow fyne.App, TeamsChat *widget.Form, tabs *container.DocTabs) *fyne.MainMenu {

	Common.ListenerTable.Refresh()
	mainMenu := fyne.NewMainMenu(
		fyne.NewMenu("Options",
			fyne.NewMenuItem("View Users", func() { CreateUserWindow(clientobj, OldWindow) }),
			fyne.NewMenuItem("Add User", func() { AddUser(clientobj, OldWindow) }),
			fyne.NewMenuItem("Delete User", func() { RemoveUser(clientobj, OldWindow) }),
		),
		fyne.NewMenu("View",
			fyne.NewMenuItem("Team Chat", func() { AddTab(tabs, TeamsChat, "Teams Chat") }),
			fyne.NewMenuItem("Listeners", func() { AddTab(tabs, Common.ListenerTable, "Active Listeners") }),
			fyne.NewMenuItem("Logging", func() { AddTab(tabs, widget.NewLabel("Logging Test"), "Logs") }),
		),
		fyne.NewMenu("Payload Builder",
			fyne.NewMenuItem("Build Payload", testfunc),
		),
		fyne.NewMenu("Help",
			fyne.NewMenuItem("Github", testfunc),
			fyne.NewMenuItem("Documentation", testfunc),
		),
	)
	return mainMenu
}

// TODO edit chat form to actually use websockets with server to send and recv messages from all logged in users.
func CreateChatForm(Username string) (*widget.Form, error) {
	//create websocket channel for chat.
	err := ObtainWebSocketConn(g_clientobj)
	if err != nil {
		log.Fatalln("[error] attempting to create websocket. panicking")
		return nil, err
	}

	teamsChatLog = widget.NewMultiLineEntry()
	teamsChatLog.Disable()
	teamsChatLog.SetMinRowsVisible(10)

	//some fucking go wizardry to ensure that we can update our team's chat if we press enter with text in the chat entry field.
	customEntryWidget = Common.NewCustomChatEntry(SendChat)

	//Teams chat entry's combined into one form.
	TeamsChat := &widget.Form{
		Items: []*widget.FormItem{
			{Text: "Teams Chat", Widget: teamsChatLog},
			{Text: Username, Widget: customEntryWidget},
		},
		OnSubmit: func() {
			SendChat(customEntryWidget.Text)
		},
		OnCancel:   ClearChatHistory,
		SubmitText: "",
		CancelText: "Clear Chat History",
	}

	return TeamsChat, nil
}

func ObtainWebSocketConn(clientobj *Common.Client) error {
	var (
		err error
	)
	clientobj.Conn, err = Common.ObtainWebSocket(clientobj)
	if err != nil {
		log.Println("[error] attempting to obtain socket connection", err)
		return err
	}
	go UpdateChat()
	return nil
}

func MainMenu(clientobj *Common.Client, OldWindow fyne.App, icon fyne.Resource, ResourcePath string) {

	//Create new window
	NewWindow := OldWindow.NewWindow("AllSecure")
	g_clientobj = clientobj
	g_username = clientobj.Username

	CreateListenerTable(clientobj, NewWindow)

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
	mainMenu := CreateMenuItems(clientobj, OldWindow, TeamsChat, tabs)

	//create initial tab
	tabs.Append(container.NewTabItemWithIcon("Teams Chat", chaticon, TeamsChat))

	// Create content for table pane
	Common.ImplantTableInit()
	ImplantTable := Common.CreateImplantTable()

	// TODO create logging pane.
	topRightContent := widget.NewLabel("Logging Pane")

	// Create a horizontal split for the top panes
	topsplit := container.NewHSplit(ImplantTable, topRightContent)

	//CenteredTabsContainer := container.New(&Common.CenteredBottomStacked{}, tabs)
	CenteredTabsContainer := container.NewStack(tabs)
	// Create a border layout with the top and bottom panes
	TopCenteredSplitContent := container.NewBorder(
		nil, CenteredTabsContainer, // No left or right panes
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
	NewWindow.SetCloseIntercept(func() {
		clientobj.Conn.Close()
		NewWindow.Close()
	})
}
