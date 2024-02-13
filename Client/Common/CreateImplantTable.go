package Common

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/theme"
	"fyne.io/fyne/v2/widget"
	"github.com/go-loremipsum/loremipsum"
	"sort"
	"strconv"
	"strings"
)

type ImplantTableHeaders struct {
	HeaderNames []string
}
type ImplantData struct {
	ImplantNum int
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

var ImplantRows []ImplantData
var Headers ImplantTableHeaders

func ImplantTableInit() {
	Headers.HeaderNames = []string{"Implant Num", "ID", "Internal IP", "External IP", "User", "Computer", "OS", "PID", "Process", "Health"}
	l := loremipsum.New()
	for i := 0; i < 500; i++ {

		row := ImplantData{
			ImplantNum: i,
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

// TODO create logic for when new implant data comes down pipeline
// Fyne labs dev blog about creating a table https://fynelabs.com/2023/10/05/user-data-sorting-with-a-fyne-table-widget/
func CreateImplantTable() *widget.Table {
	t := widget.NewTableWithHeaders(func() (int, int) { return len(ImplantRows), len(Headers.HeaderNames) },
		func() fyne.CanvasObject { l := widget.NewLabel(""); return l },
		func(id widget.TableCellID, o fyne.CanvasObject) {
			l := o.(*widget.Label)
			l.Truncation = fyne.TextTruncateEllipsis
			switch id.Col {
			case 0:
				l.Truncation = fyne.TextTruncateOff
				l.SetText(strconv.Itoa(ImplantRows[id.Row].ImplantNum))
			case 1:
				l.Truncation = fyne.TextTruncateOff
				l.SetText(ImplantRows[id.Row].ID)
			case 2:
				l.SetText(ImplantRows[id.Row].InternalIP)
			case 3:
				l.SetText(ImplantRows[id.Row].ExternalIP)
			case 4:
				l.SetText(ImplantRows[id.Row].User)
			case 5:
				l.SetText(ImplantRows[id.Row].Computer)
			case 6:
				l.SetText(ImplantRows[id.Row].OS)
			case 7:
				l.SetText(strconv.Itoa(ImplantRows[id.Row].PID))
			case 8:
				l.SetText(ImplantRows[id.Row].Process)
			case 9:
				l.SetText(ImplantRows[id.Row].Health)
			}
		},
	)
	t.SetColumnWidth(0, 125)
	t.SetColumnWidth(1, 125)
	t.SetColumnWidth(2, 150)
	t.SetColumnWidth(3, 150)
	t.SetColumnWidth(4, 150)
	t.SetColumnWidth(5, 150)
	t.SetColumnWidth(6, 125)
	t.SetColumnWidth(7, 100)
	t.SetColumnWidth(8, 125)
	t.SetColumnWidth(9, 100)

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
			switch id.Col {
			case 0:
				b.SetText(Headers.HeaderNames[id.Col])
				switch sorts[0] {
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			case 1:
				b.SetText(Headers.HeaderNames[id.Col])
				switch sorts[1] {
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			case 2:
				b.SetText(Headers.HeaderNames[id.Col])
				switch sorts[2] {
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			case 3:
				b.SetText(Headers.HeaderNames[id.Col])
				switch sorts[3] {
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			case 4:
				b.SetText(Headers.HeaderNames[id.Col])
				switch sorts[4] {
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			case 5:
				b.SetText(Headers.HeaderNames[id.Col])
				switch sorts[5] {
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			case 6:
				b.SetText(Headers.HeaderNames[id.Col])
				switch sorts[6] {
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			case 7:
				b.SetText(Headers.HeaderNames[id.Col])
				switch sorts[7] {
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			case 8:
				b.SetText(Headers.HeaderNames[id.Col])
				switch sorts[8] {
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			case 9:
				b.SetText(Headers.HeaderNames[id.Col])
				switch sorts[8] {
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			}

			b.Importance = widget.MediumImportance
			b.OnTapped = func() {
				ApplySort(id.Col, t)
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

var sorts = [9]Direction{}

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

	sort.Slice(ImplantRows, func(i, j int) bool {
		//grab two rows at indexes I & J
		a := ImplantRows[i]
		b := ImplantRows[j]

		//compare them for no sorting
		if order == SortOff {
			return a.ImplantNum > b.ImplantNum
		}
		/*
			Column Order
			opts.HeaderNames = []string{"Implant Num", "ID", "Internal IP", "External IP", "User", "Computer", "OS", "PID", "Process", "Health"}
		*/
		//sort each column within each row that has been selected cases 1 - 9
		switch col {
		case 1:
			if order == SortAsc {
				return strings.Compare(a.ID, b.ID) < 0
			}
			return strings.Compare(a.ID, b.ID) > 0
		case 2:
			if order == SortAsc {
				return strings.Compare(a.InternalIP, b.InternalIP) < 0
			}
			return strings.Compare(a.InternalIP, b.InternalIP) > 0
		case 3:
			if order == SortAsc {
				return strings.Compare(a.ExternalIP, b.ExternalIP) < 0
			}
			return strings.Compare(a.ExternalIP, b.ExternalIP) > 0
		case 4:
			if order == SortAsc {
				return strings.Compare(a.User, b.User) < 0
			}
			return strings.Compare(a.User, b.User) > 0
		case 5:
			if order == SortAsc {
				return strings.Compare(a.Computer, b.Computer) < 0
			}
			return strings.Compare(a.Computer, b.Computer) > 0
		case 6:
			if order == SortAsc {
				return strings.Compare(a.OS, b.OS) < 0
			}
			return strings.Compare(a.OS, b.OS) > 0
		case 7:
			if order == SortAsc {
				return a.PID < b.PID
			}
			return a.PID > b.PID
		case 8:
			if order == SortAsc {
				return strings.Compare(a.Process, b.Process) < 0
			}
			return strings.Compare(a.Process, b.Process) > 0
		case 9:
			if order == SortAsc {
				return strings.Compare(a.Health, b.Health) < 0
			}
			return strings.Compare(a.Health, b.Health) > 0
		default:
			if order == SortDesc {
				return a.ImplantNum > b.ImplantNum
			}
			return a.ImplantNum < b.ImplantNum
		}
	})

	t.Refresh()
}
