package Common

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/widget"
	"github.com/go-loremipsum/loremipsum"
	"strconv"
)

type ImplantTableOptions struct {
	HeaderNames []string
}
type ImplantRowData struct {
	ID         string
	InternalIP string
	ExternalIP string
	User       string
	Computer   string
	OS         string
	PID        int
	Process    string
	Health     string
}

var ImplantRows []ImplantRowData
var opts ImplantTableOptions

func Init() {
	opts.HeaderNames = []string{"ID", "Internal IP", "External IP", "User", "Computer", "OS", "PID", "Process", "Health"}
	l := loremipsum.New()
	for i := 0; i < 500; i++ {

		row := ImplantRowData{
			ID:         l.Word(),
			InternalIP: l.Word(),
			ExternalIP: l.Word(),
			User:       l.Word(),
			Computer:   l.Word(),
			OS:         l.Word(),
			PID:        i,
			Process:    l.Word(),
			Health:     l.Word(),
		}

		ImplantRows = append(ImplantRows, row)
	}
}

func CreateImplantTable() *widget.Table {
	t := widget.NewTableWithHeaders(func() (int, int) { return len(ImplantRows), len(opts.HeaderNames) },
		func() fyne.CanvasObject { l := widget.NewLabel(""); return l },
		func(id widget.TableCellID, o fyne.CanvasObject) {
			l := o.(*widget.Label)
			l.Truncation = fyne.TextTruncateEllipsis
			switch id.Col {
			case 0:
				l.Truncation = fyne.TextTruncateOff
				l.SetText(ImplantRows[id.Row].ID)
			case 1:
				l.SetText(ImplantRows[id.Row].InternalIP)
			case 2:
				l.SetText(ImplantRows[id.Row].ExternalIP)
			case 3:
				l.SetText(ImplantRows[id.Row].User)
			case 4:
				l.SetText(ImplantRows[id.Row].Computer)
			case 5:
				l.SetText(ImplantRows[id.Row].OS)
			case 6:
				l.SetText(strconv.Itoa(ImplantRows[id.Row].PID))
			case 7:
				l.SetText(ImplantRows[id.Row].Process)
			case 8:
				l.SetText(ImplantRows[id.Row].Health)
			}
		},
	)
	t.SetColumnWidth(0, 50)
	t.SetColumnWidth(1, 125)
	t.SetColumnWidth(2, 500)
	t.SetColumnWidth(3, 500)
	t.SetColumnWidth(4, 500)
	t.SetColumnWidth(5, 500)
	t.SetColumnWidth(6, 500)
	t.SetColumnWidth(7, 500)
	t.SetColumnWidth(8, 500)

	t.CreateHeader = func() fyne.CanvasObject {
		return widget.NewButton("000", func() {})
	}
	t.UpdateHeader = func(id widget.TableCellID, o fyne.CanvasObject) {
		b := o.(*widget.Button)
		if id.Col == -1 {
			b.SetText(strconv.Itoa(id.Row))
			b.Importance = widget.LowImportance
			b.Disable()
		} else {
			b.SetText(opts.HeaderNames[id.Col])
			b.Importance = widget.MediumImportance
			b.OnTapped = func() {
				return
			}
			b.Enable()
			b.Refresh()
		}
	}

	return t
}

type Direction int

const (
	SortOff Direction = iota
	SortAsc
	SortDesc
)

var sorts = [3]Direction{}

/*
func ApplySort(col int, t *widget.Table) {
	order := sorts[col]
	order++
	if order > SortDesc {
		order = SortOff
	}

	//reset all and assign tapped sort
	for i := 0; i < 3; i++ {
		sorts[i] = SortOff
	}
	sorts[col] = order

	sort.Slice(rows, func(i, j int) bool {
		a := rows[i]
		b := rows[j]
		if order == SortOff {
			return a.id > b.id
		}

		switch col {
		case 1:
			if order == SortAsc {
				return strings.Compare(a.name, b.name) < 0
			}
			return strings.Compare(a.name, b.name) > 0
		case 2:
			if order == SortAsc {
				return strings.Compare(a.description, b.description) < 0
			}
			return strings.Compare(a.description, b.description) > 0
		case default:
			if order == SortDesc {
				return a.id > b.id
			}
			return a.id < b.id
		}
	})

	t.Refresh()
}


*/
