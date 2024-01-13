package main

import (
	"Client/Common"
	"fmt"
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
	"time"
)

func testfunc() {

}

func MainMenu(Username string, OldWindow fyne.App, icon fyne.Resource) {

	//Create new window
	myWindow := OldWindow.NewWindow("TabContainer Widget")

	//Create new teams chat
	TeamsChatLog := widget.NewMultiLineEntry()
	TeamsChatLog.Disable()
	//some fucking go wizardry to ensure that we can update our team's chat if we press enter with text in the chat entry field.
	OnEnterKeyPress := func(EnteredText string) {
		prevtext := TeamsChatLog.Text
		date := time.Now()
		TeamsChatLog.SetText(fmt.Sprintf("%s\n%s [%s] :: %s", prevtext, date.Format("2006-01-02 15:04:05"), Username, EnteredText))
	}
	CustomEntryWidget := Common.NewCustomChatEntry(OnEnterKeyPress)

	//Teams chat entry's combined into one form.
	TeamsChat := &widget.Form{
		Items: []*widget.FormItem{
			{Text: "Teams Chat", Widget: TeamsChatLog},
			{Text: Username, Widget: CustomEntryWidget},
		},
		OnSubmit: func() {
			prevtext := TeamsChatLog.Text
			date := time.Now()
			TeamsChatLog.SetText(fmt.Sprintf("%s\n%s [%s] :: %s", prevtext, date.Format("2006-01-02 15:04:05"), Username, CustomEntryWidget.Text))
		},
		OnCancel:   nil,
		SubmitText: "",
		CancelText: "",
	}

	//creating all menu items.
	mainMenu := fyne.NewMainMenu(
		fyne.NewMenu("Options",
			fyne.NewMenuItem("AddUser", testfunc),
			fyne.NewMenuItem("DeleteUser", testfunc),
		),
		fyne.NewMenu("View",
			fyne.NewMenuItem("Team Chat", testfunc),
			fyne.NewMenuItem("Listeners", testfunc),
			fyne.NewMenuItem("Logging", testfunc),
		),
		fyne.NewMenu("Payload Builder",
			fyne.NewMenuItem("Build Payload", testfunc),
		),
		fyne.NewMenu("Help",
			fyne.NewMenuItem("Github", testfunc),
			fyne.NewMenuItem("Documentation", testfunc),
		),
	)

	// Create content for each pane

	Common.Init()
	ImplantTable := Common.CreateImplantTable()

	topRightContent := widget.NewLabel("Log Channel")

	// Create a horizontal split for the top panes
	topsplit := container.NewHSplit(ImplantTable, topRightContent)
	//bottomsplit := container.NewVSplit(topsplit, TeamsChat)

	// Create a border layout with the top and bottom panes
	content := container.NewBorder(
		nil, nil, // No left or right panes
		nil, nil, // Top and bottom panes
		topsplit, // Center pane (topSplit)
	)
	content2 := container.NewVSplit(content, TeamsChat)
	//content3 := container.NewBorder(nil, nil, nil, nil, content2)

	myWindow.SetMainMenu(mainMenu)

	myWindow.SetContent(content2)
	myWindow.Resize(fyne.NewSize(600, 400))
	myWindow.SetIcon(icon)

	myWindow.Show()
}

//TODO create tabs for each view
/*

	tabs := container.NewAppTabs(
		container.NewTabItem("Tab 1", widget.NewLabel("Hello")),
		container.NewTabItem("Tab 2", widget.NewLabel("World!")),
	)
*/
