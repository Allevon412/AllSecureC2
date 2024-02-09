package mainmenu

import (
	"Client/Common"
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
	"log"
)

var (
	teamsChatLog      *widget.Entry
	customEntryWidget *Common.CustomChatEntry
	username          string
	chaticon          fyne.Resource
)

func CreateMenuItems(clientobj *Common.Client, OldWindow fyne.App, TeamsChat *widget.Form, tabs *container.DocTabs) *fyne.MainMenu {
	mainMenu := fyne.NewMainMenu(
		fyne.NewMenu("Options",
			fyne.NewMenuItem("View Users", func() { CreateUserWindow(clientobj, OldWindow) }),
			fyne.NewMenuItem("Add User", func() { AddUser(clientobj, OldWindow) }),
			fyne.NewMenuItem("Delete User", func() { CreateUserWindow(clientobj, OldWindow) }),
		),
		fyne.NewMenu("View",
			fyne.NewMenuItem("Team Chat", func() { AddTab(tabs, TeamsChat, "Teams Chat") }),
			fyne.NewMenuItem("Listeners", func() { AddTab(tabs, widget.NewLabel("Listeners Test"), "Active Listeners") }),
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

func CreateChatForm(Username string) *widget.Form {
	teamsChatLog = widget.NewMultiLineEntry()
	teamsChatLog.Disable()
	teamsChatLog.SetMinRowsVisible(10)

	//some fucking go wizardry to ensure that we can update our team's chat if we press enter with text in the chat entry field.
	customEntryWidget = Common.NewCustomChatEntry(UpdateChat)

	//Teams chat entry's combined into one form.
	TeamsChat := &widget.Form{
		Items: []*widget.FormItem{
			{Text: "Teams Chat", Widget: teamsChatLog},
			{Text: Username, Widget: customEntryWidget},
		},
		OnSubmit: func() {
			UpdateChat(customEntryWidget.Text)
		},
		OnCancel:   nil,
		SubmitText: "",
		CancelText: "",
	}

	return TeamsChat
}

func MainMenu(clientobj *Common.Client, OldWindow fyne.App, icon fyne.Resource, ResourcePath string) {
	var err error
	//Create new window
	NewWindow := OldWindow.NewWindow("AllSecure")
	username = clientobj.Username

	//Create new teams chat
	TeamsChat := CreateChatForm(username)

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
}
