package main

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
	"image/color"
)

// TODO: Make authentication function
func Authenticate() {

}

// TODO: Make Setup Function
func InitAllSecure() {

}

func NewClientWindow() {
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

func main() {

	NewClientWindow()

}
