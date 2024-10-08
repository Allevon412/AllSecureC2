package Common

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/driver/desktop"
	"fyne.io/fyne/v2/widget"
)

const (
	UserCell CellType = iota
	ImplantCell
	ListenerCell
)

type (
	CellType int

	// extending the natural entry widget to have the functionality to send text by pressing enter.
	CustomChatEntry struct {
		widget.Entry
		FormSubmitFunc func(CurrentText string)
	}

	CustomEntry struct {
		widget.Entry
		FormSubmitFunc func()
	}

	CustomTableLabel struct {
		widget.Label
		LabelWindowAdditionFunction func(e *fyne.PointEvent)
		Row                         int
		Col                         int
		LabelType                   CellType
	}

	CustomLabelWidget struct {
		widget.Label
		DoNothing func(event *desktop.MouseEvent)
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

// ExtendedList is a custom widget that extends the List widget.
type ExtendedList struct {
	*widget.List
	lines int
}
