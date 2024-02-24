package Common

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/theme"
	"fyne.io/fyne/v2/widget"
	"sort"
	"strconv"
	"strings"
)

var ListenerSorts = [7]Direction{}

type ListenerTableHeaders struct {
	HeaderNames []string
}
type ListenerData struct {
	ListenerName string
	Protocol     string
	HOST         string
	PortBind     int
	PortConn     string
}

var (
	ListenerHeaders      ListenerTableHeaders
	ListenerTableEntries []ListenerTableData
	ListenerTable        *widget.Table
)

func ListenerTableInit() {
	ListenerHeaders.HeaderNames = []string{"Listener Name", "Protocol", "HOST", "Port Bind", "Port Conn", "UserID", "Username"}
}

func ApplyListenerSort(col int, t *widget.Table) {
	order := ListenerSorts[col]
	order++
	if order > SortDesc {
		order = SortOff
	}

	//reset all and assign tapped sort
	for i := 0; i < 3; i++ {
		ListenerSorts[i] = SortOff
	}
	ListenerSorts[col] = order

	sort.Slice(ListenerTableEntries, func(i, j int) bool {
		a := ListenerTableEntries[i]
		b := ListenerTableEntries[j]

		//compare them for no sorting
		if order == SortOff {
			return strings.Compare(a.ListenerName, b.ListenerName) < 0
		}
		switch col {
		case 1:
			if order == SortAsc {
				return strings.Compare(a.Protocol, b.Protocol) < 0
			}
			return strings.Compare(a.Protocol, b.Protocol) > 0
		case 2:
			if order == SortAsc {
				return strings.Compare(a.HOST, b.HOST) < 0
			}
			return strings.Compare(a.HOST, b.HOST) > 0
		case 3:
			if order == SortAsc {
				return a.PortBind < b.PortBind
			}
			return a.PortBind > b.PortBind
		case 4:
			if order == SortAsc {
				return strings.Compare(a.PortConn, b.PortConn) < 0
			}
			return strings.Compare(a.PortConn, b.PortConn) > 0
		case 6:
			if order == SortAsc {
				return strings.Compare(a.UserName, b.UserName) < 0
			}
			return strings.Compare(a.UserName, b.UserName) > 0
		case 5:
			if order == SortAsc {
				return a.UserID < b.UserID
			}
			return a.UserID > b.UserID

		default:
			if order == SortDesc {
				return strings.Compare(a.ListenerName, b.ListenerName) < 0
			}
			return strings.Compare(a.ListenerName, b.ListenerName) > 0
		}

	})
	t.Refresh()
}

func CreateListenerTableObject(PopUpMenu *widget.PopUpMenu) *widget.Table {
	ListenerTableInit()
	t := widget.NewTableWithHeaders(
		func() (int, int) { return len(ListenerTableEntries), len(ListenerHeaders.HeaderNames) },
		func() fyne.CanvasObject {
			l := NewCustomUserTableLabel(
				func(e *fyne.PointEvent) {
					PopUpMenu.ShowAtPosition(e.AbsolutePosition)
					PopUpMenu.Show()
				})
			return l
		},
		func(id widget.TableCellID, o fyne.CanvasObject) {
			l := o.(*CustomUserTableLabel)
			l.Truncation = fyne.TextTruncateEllipsis
			switch id.Col {
			case 0:
				l.Truncation = fyne.TextTruncateOff
				l.SetText(ListenerTableEntries[id.Row].ListenerName)
			case 1:
				l.Truncation = fyne.TextTruncateOff
				l.SetText(ListenerTableEntries[id.Row].Protocol)
			case 2:
				l.Truncation = fyne.TextTruncateOff
				l.SetText(ListenerTableEntries[id.Row].HOST)
			case 3:
				l.Truncation = fyne.TextTruncateOff
				l.SetText(strconv.Itoa(ListenerTableEntries[id.Row].PortBind))
			case 4:
				l.Truncation = fyne.TextTruncateOff
				l.SetText(ListenerTableEntries[id.Row].PortConn)
			case 5:
				l.Truncation = fyne.TextTruncateOff
				l.SetText(strconv.Itoa(ListenerTableEntries[id.Row].UserID))
			case 6:
				l.Truncation = fyne.TextTruncateOff
				l.SetText(ListenerTableEntries[id.Row].UserName)
			}
		},
	)

	t.SetColumnWidth(0, 125)
	t.SetColumnWidth(1, 125)
	t.SetColumnWidth(2, 125)
	t.SetColumnWidth(3, 200)
	t.SetColumnWidth(4, 200)
	t.SetColumnWidth(5, 200)
	t.SetColumnWidth(6, 200)

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
				b.SetText(ListenerHeaders.HeaderNames[id.Col])
				switch ListenerSorts[0] {
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			case 1:
				b.SetText(ListenerHeaders.HeaderNames[id.Col])
				switch ListenerSorts[1] {
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			case 2:
				b.SetText(ListenerHeaders.HeaderNames[id.Col])
				switch ListenerSorts[2] {
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			case 3:
				b.SetText(ListenerHeaders.HeaderNames[id.Col])
				switch ListenerSorts[3] {
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			case 4:
				b.SetText(ListenerHeaders.HeaderNames[id.Col])
				switch ListenerSorts[4] {
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			case 5:
				b.SetText(ListenerHeaders.HeaderNames[id.Col])
				switch ListenerSorts[5] {
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			case 6:
				b.SetText(ListenerHeaders.HeaderNames[id.Col])
				switch ListenerSorts[6] {
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
				ApplyListenerSort(id.Col, t)
			}
			b.Enable()
			b.Refresh()
		}
	}

	return t
}
