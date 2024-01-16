package Common

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
)

type Widget interface {
	fyne.CanvasObject
	CreateRenderer() fyne.WidgetRenderer
}

type ChatLog struct {
	widget.BaseWidget
	Chat *widget.TextGrid
}

func NewChatLogWidget(title, comment string) *ChatLog {
	item := &ChatLog{
		BaseWidget: widget.BaseWidget{},
		Chat:       widget.NewTextGrid(),
	}

	item.ExtendBaseWidget(item)
	return item
}

func (item *ChatLog) CreateRenderer() fyne.WidgetRenderer {

	c := container.NewBorder(nil, nil, nil, nil, item.Chat)
	return widget.NewSimpleRenderer(c)
}
