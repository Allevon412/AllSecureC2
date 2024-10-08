package Common

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/driver/desktop"
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

func NewCustomCredentialEntry(SubmitFunc func()) *CustomEntry {

	entry := &CustomEntry{widget.Entry{
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

func (cce *CustomEntry) TypedKey(key *fyne.KeyEvent) {
	if key.Name == fyne.KeyEnter || key.Name == fyne.KeyReturn {
		cce.FormSubmitFunc()
	}
	cce.Entry.TypedKey(key)
}

func NewCustomTableLabel(CustomFunc func(e *fyne.PointEvent)) *CustomTableLabel {
	label := &CustomTableLabel{
		Label:                       widget.Label{},
		LabelWindowAdditionFunction: CustomFunc,
		Row:                         0,
		Col:                         0,
	}
	label.ExtendBaseWidget(label)
	return label
}

func (cl *CustomTableLabel) TappedSecondary(e *fyne.PointEvent) {

	cl.LabelWindowAdditionFunction(e)
	switch cl.LabelType {
	case UserCell:
		SelectedUser = SelectedCell{
			Row: cl.Row,
			Col: cl.Col,
		}
		break
	case ImplantCell:
		SelectedImplant = SelectedCell{
			Row: cl.Row,
			Col: cl.Col,
		}
		break
	case ListenerCell:
		SelectedListener = SelectedCell{
			Row: cl.Row,
			Col: cl.Col,
		}
		break
	}

}

func NewCustomLabelWidget(DoNothing func(m *desktop.MouseEvent)) *CustomLabelWidget {
	label := &CustomLabelWidget{
		Label:     widget.Label{},
		DoNothing: DoNothing,
	}
	label.ExtendBaseWidget(label)
	return label
}

func (cl *CustomLabelWidget) MouseIn(m *desktop.MouseEvent) {
	cl.DoNothing(m)
}
func (cl *CustomLabelWidget) MouseOut() {
	cl.DoNothing(nil)
}
func (cl *CustomLabelWidget) MouseMoved(m *desktop.MouseEvent) {
	cl.DoNothing(m)
}

// NewExtendedList creates a new instance of ExtendedList.
func NewExtendedList(lines int, length func() int, create func() fyne.CanvasObject, update func(id widget.ListItemID, obj fyne.CanvasObject)) *ExtendedList {
	list := &ExtendedList{
		List:  widget.NewList(length, create, update),
		lines: lines,
	}
	list.ExtendBaseWidget(list)
	return list
}

// MinSize returns the custom minimum size for the ExtendedList widget.
func (l *ExtendedList) MinSize() fyne.Size {
	itemHeight := l.CreateItem().MinSize().Height
	return fyne.NewSize(l.List.MinSize().Width, itemHeight*float32(l.lines))
}
