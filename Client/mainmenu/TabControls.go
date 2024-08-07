package mainmenu

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/theme"
	"fyne.io/fyne/v2/widget"
)

func AddTab(tabs *container.DocTabs, content fyne.CanvasObject, TabName string) {
	var NewTabItem *container.TabItem
	//create a new tab item with the passed parameters/
	switch content.(type) {
	case *widget.Form:
		NewTabItem = container.NewTabItemWithIcon(TabName, chaticon, content)
		break
	case *widget.Label:
		NewTabItem = container.NewTabItemWithIcon(TabName, theme.FyneLogo(), content)
		break
	case *widget.Table:
		NewTabItem = container.NewTabItemWithIcon(TabName, theme.FyneLogo(), content)
		break
	case *container.Split:
		NewTabItem = container.NewTabItemWithIcon(TabName, chaticon, content)
		break
	default:
		NewTabItem = container.NewTabItemWithIcon(TabName, theme.FyneLogo(), content)
		break
	}

	content.Resize(fyne.NewSize(100, 200))

	//obtain the items in our doc tabs container
	items := tabs.Items
	//cycle through them.
	for _, item := range items {
		// if this tab is already open, then just return.
		if item.Text == TabName && item.Disabled() == false {
			return
		}
	}
	//otherwise add it.
	tabs.Append(NewTabItem)
}
