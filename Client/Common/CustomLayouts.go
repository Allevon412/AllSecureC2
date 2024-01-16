package Common

import "fyne.io/fyne/v2"

type Layout interface {
	Layout([]fyne.CanvasObject, fyne.Size)
	MinSize(objects []fyne.CanvasObject) fyne.Size
}

type CenteredBottomStacked struct {
}

func (c *CenteredBottomStacked) MinSize(objects []fyne.CanvasObject) fyne.Size {
	minSize := fyne.NewSize(0, 0)
	for _, child := range objects {
		if !child.Visible() {
			continue
		}

		minSize = minSize.Max(child.MinSize())
	}

	return minSize
}

// reizes the child object to the size of the container. This will stack all children ontop of each other currently
func (c *CenteredBottomStacked) Layout(objects []fyne.CanvasObject, containerSize fyne.Size) {
	pos := fyne.NewPos(0, containerSize.Height-c.MinSize(objects).Height)
	for _, child := range objects {
		child.Resize(containerSize)
		child.Move(pos)
	}
}
