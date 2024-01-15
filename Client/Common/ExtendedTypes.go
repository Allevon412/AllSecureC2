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

	CustomCredentialsEntry struct {
		widget.Entry
		FormSubmitFunc func()
	}

	//custom theme for allsecure so we do not have the greyed out team's chat box.
	AllSecureTheme struct{}

	TableOptions struct {
		Headers    []string
		CellWidth  float32
		CellHeight float32
	}
)

// asserting the custom theme
var _ fyne.Theme = (*AllSecureTheme)(nil)
