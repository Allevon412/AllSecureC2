package Common

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/theme"
	"image/color"
)

// the custom theme code.
func (m AllSecureTheme) Color(name fyne.ThemeColorName, variant fyne.ThemeVariant) color.Color {

	//ensuring our text shows clearly.
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
