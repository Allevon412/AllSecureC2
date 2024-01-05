package main

import (
	"errors"
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
	"image/color"
	"os"
)

// TODO: Make authentication function
func Authenticate() {

}

// TODO: Make Setup Function
func (config ClientConfig) InitAllSecure() {

	//check if database exists
	_, err := os.Stat(config.ProjectDir + "\\Database")
	if errors.Is(err, os.ErrNotExist) {
		os.Create(config.ProjectDir + "\\Database")
	}
	_, err := os.Stat(config.ProjectDir + "\\Database\\" + "AllSecure.db")
	if errors.Is(err, os.ErrNotExist) {

	}
}

func (config ClientConfig) NewClientWindow() {
	a := app.New()
	w := a.NewWindow("AllSecure")
	myCanvas := w.Canvas()

	blue := color.NRGBA{R: 0, G: 0, B: 180, A: 255}
	rect := canvas.NewRectangle(blue)
	myCanvas.SetContent(rect)

	Config := widget.NewLabel("Set Configuration")
	w.SetContent(container.NewVBox(
		Config,
		widget.NewButton("Set Configuration", func() {
			Config.SetText("Welcome :)")
		}),
	))

	w.Resize(fyne.NewSize(1000, 1000))
	w.ShowAndRun()
}

func (config ClientConfig) main() {
	config.NewClientWindow()

}
