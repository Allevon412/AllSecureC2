package Common

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/widget"
)

type (
	// extending the natural entry widget to have the functionality to send text by pressing enter.
	CustomChatEntry struct {
		widget.Entry
		FormSubmitFunc func(CurrentText string)
	}

	//custom theme for allsecure so we do not have the greyed out team's chat box.
	AllSecureTheme struct{}

	TableOptions struct {
		Headers    []string
		CellWidth  float32
		CellHeight float32
	}

	CustomTable struct {
		widget.Table
		TableOpts TableOptions
	}
)

// asserting the custome theme
var _ fyne.Theme = (*AllSecureTheme)(nil)
var _ fyne.Widget = (*CustomTable)(nil)
