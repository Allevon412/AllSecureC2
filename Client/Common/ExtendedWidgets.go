package Common

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/widget"
)

// our custom entry widget button for our chat.
func NewCustomChatEntry(SubmitFunc func(CurrentText string)) *CustomChatEntry {
	entry := &CustomChatEntry{widget.Entry{
		DisableableWidget: widget.DisableableWidget{},
		Text:              "",
		TextStyle:         fyne.TextStyle{},
		PlaceHolder:       "",
		OnChanged:         nil,
		OnSubmitted:       nil,
		Password:          false,
		MultiLine:         false,
		Wrapping:          0,
		Scroll:            0,
		Validator:         nil,
		CursorRow:         0,
		CursorColumn:      0,
		OnCursorChanged:   nil,
		ActionItem:        nil,
	}, SubmitFunc}
	entry.ExtendBaseWidget(entry)
	return entry
}

// the code to ensure that when we press enter, the message is sent to the form submit as well.
func (ce *CustomChatEntry) TypedKey(key *fyne.KeyEvent) {
	if key.Name == fyne.KeyEnter || key.Name == fyne.KeyReturn {
		ce.FormSubmitFunc(ce.Text)
	}
	ce.Entry.TypedKey(key)
}

func NewCustomCredentialEntry(SubmitFunc func()) *CustomCredentialsEntry {

	entry := &CustomCredentialsEntry{widget.Entry{
		DisableableWidget: widget.DisableableWidget{},
		Text:              "",
		TextStyle:         fyne.TextStyle{},
		PlaceHolder:       "",
		OnChanged:         nil,
		OnSubmitted:       nil,
		Password:          false,
		MultiLine:         false,
		Wrapping:          0,
		Scroll:            0,
		Validator:         nil,
		CursorRow:         0,
		CursorColumn:      0,
		OnCursorChanged:   nil,
		ActionItem:        nil,
	}, SubmitFunc}

	entry.ExtendBaseWidget(entry)
	return entry
}

func (cce *CustomCredentialsEntry) TypedKey(key *fyne.KeyEvent) {
	if key.Name == fyne.KeyEnter || key.Name == fyne.KeyReturn {
		cce.FormSubmitFunc()
	}
	cce.Entry.TypedKey(key)
}

func NewCustomUserTableLabel(CustomFunc func(e *fyne.PointEvent)) *CustomUserTableLabel {
	label := &CustomUserTableLabel{
		Label:                       widget.Label{},
		LabelWindowAdditionFunction: CustomFunc,
	}
	label.ExtendBaseWidget(label)
	return label
}
func (cl *CustomUserTableLabel) TappedSecondary(e *fyne.PointEvent) {

	cl.LabelWindowAdditionFunction(e)
}
