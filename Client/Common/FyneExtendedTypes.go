package Common

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/theme"
	"fyne.io/fyne/v2/widget"
	"image/color"
)

type (
	MyEntryStruct struct {
		widget.Entry
	}

	AllSecureTheme struct{}
)

var _ fyne.Theme = (*AllSecureTheme)(nil)

func (m AllSecureTheme) Color(name fyne.ThemeColorName, variant fyne.ThemeVariant) color.Color {

	if name == theme.ColorNameDisabled {
		return color.White
	}

	return theme.DefaultTheme().Color(name, variant)
}

func (m AllSecureTheme) Icon(name fyne.ThemeIconName) fyne.Resource {

	return theme.DefaultTheme().Icon(name)
}

func (m AllSecureTheme) Font(style fyne.TextStyle) fyne.Resource {
	return theme.DefaultTheme().Font(style)
}

func (m AllSecureTheme) Size(name fyne.ThemeSizeName) float32 {
	return theme.DefaultTheme().Size(name)
}
