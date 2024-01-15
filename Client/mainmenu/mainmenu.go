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
	err               error
)

func MainMenu(Username string, OldWindow fyne.App, icon fyne.Resource, ResourcePath string) {

	//Create new window
	NewWindow := OldWindow.NewWindow("TabContainer Widget")
	username = Username

	//Create new teams chat
	teamsChatLog = widget.NewMultiLineEntry()
	teamsChatLog.Disable()

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

	//load chat icon
	chaticon, err = fyne.LoadResourceFromPath(ResourcePath + "chat.ico")
	if err != nil {
		log.Println("[error] attempting to load chat icon", err)
	}

	//create new document tab container
	tabs := container.NewDocTabs()

	//creating all menu items.
	mainMenu := fyne.NewMainMenu(
		fyne.NewMenu("Options",
			fyne.NewMenuItem("AddUser", testfunc),
			fyne.NewMenuItem("DeleteUser", testfunc),
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

	// Create content for table pane
	Common.Init()
	ImplantTable := Common.CreateImplantTable()

	// TODO create logging pane.
	topRightContent := widget.NewLabel("Log Channel")

	// Create a horizontal split for the top panes
	topsplit := container.NewHSplit(ImplantTable, topRightContent)

	// Create a border layout with the top and bottom panes
	TopCenteredSplitContent := container.NewBorder(
		nil, nil, // No left or right panes
		nil, nil, // Top and bottom panes
		topsplit, // Center pane (topSplit)
	)

	BottomWindow := container.NewVSplit(TopCenteredSplitContent, tabs)
	//content3 := container.NewBorder(nil, nil, nil, nil, content2)

	NewWindow.SetMainMenu(mainMenu)

	NewWindow.SetContent(BottomWindow)
	NewWindow.Resize(fyne.NewSize(1920, 1080))
	NewWindow.SetIcon(icon)

	NewWindow.Show()
}
