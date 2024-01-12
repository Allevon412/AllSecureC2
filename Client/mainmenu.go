package main

import (
	"fmt"
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/layout"
	"fyne.io/fyne/v2/widget"
	"time"
)

func testfunc() {

}

func MainMenu(Username string, OldWindow fyne.App, icon fyne.Resource) {

	myWindow := OldWindow.NewWindow("TabContainer Widget")

	TeamsChatLog := widget.NewMultiLineEntry()
	TeamsChatLog.Disable()

	ChatEntry := widget.NewEntry()

	TeamsChat := &widget.Form{
		Items: []*widget.FormItem{
			{Text: Username, Widget: ChatEntry},
		},
		OnSubmit: func() {
			prevtext := TeamsChatLog.Text
			date := time.Now()
			TeamsChatLog.SetText(fmt.Sprintf("%s\n%s [%s] :: %s", prevtext, date.Format("2006-01-02 15:04:05"), Username, ChatEntry.Text))
		},
		OnCancel:   nil,
		SubmitText: "",
		CancelText: "",
	}

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
	topLeftContent := widget.NewLabel("Active Implants")
	topRightContent := widget.NewLabel("Log Channel")
	bottomContent := widget.NewLabel("Team Chat")

	chatbox := container.New(layout.NewAdaptiveGridLayout(2), TeamsChatLog, TeamsChat)

	//chatbox := container.NewAppTabs(TeamsChatLog, TeamsChat)
	// := container.NewGridWithRows(2, , layout.NewSpacer(), )
	bottomstack := container.NewStack(bottomContent, chatbox)
	// Create a horizontal split for the top panes
	topsplit := container.NewHSplit(topLeftContent, topRightContent)
	bottomsplit := container.NewVSplit(topsplit, bottomstack)

	// Create a border layout with the top and bottom panes
	content := container.NewBorder(
		nil, nil, // No left or right panes
		nil, nil, // Top and bottom panes
		topsplit, bottomsplit, // Center pane (topSplit)
	)
	myWindow.SetMainMenu(mainMenu)

	myWindow.SetContent(content)
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
