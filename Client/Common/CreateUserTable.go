package Common

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/theme"
	"fyne.io/fyne/v2/widget"
	"sort"
	"strconv"
	"strings"
)

var UserSorts = [5]Direction{}

type UserTableHeaders struct {
	HeaderNames []string
}
type UserData struct {
	UserID          int
	UserName        string
	Admin           bool
	ActiveImplants  string
	ActiveListeners string
}

var (
	UserHeaders      UserTableHeaders
	UserTableEntries []UserTableData
	UserTable        *widget.Table
)

func UserTableInit() {
	UserHeaders.HeaderNames = []string{"User ID", "Username", "Administrator", "Active Implants", "Active Listeners"}
}

func ApplyUserSort(col int, t *widget.Table) {
	order := UserSorts[col]
	order++
	if order > SortDesc {
		order = SortOff
	}

	//reset all and assign tapped sort
	for i := 0; i < 3; i++ {
		UserSorts[i] = SortOff
	}
	UserSorts[col] = order

	sort.Slice(UserTableEntries, func(i, j int) bool {
		a := UserTableEntries[i]
		b := UserTableEntries[j]

		//compare them for no sorting
		if order == SortOff {
			return strings.Compare(a.Username, b.Username) < 0
		}
		switch col {
		case 1:
			if order == SortAsc {
				return a.ID < b.ID
			}
			return a.ID > b.ID
		case 2:
			if order == SortAsc {
				return a.Admin < b.Admin
			}
			return a.Admin > b.Admin
		case 3:
			if order == SortAsc {
				return strings.Compare(a.ActiveImplants, b.ActiveImplants) < 0
			}
			return strings.Compare(a.ActiveImplants, b.ActiveImplants) > 0
		case 4:
			if order == SortAsc {
				return strings.Compare(a.ActiveListeners, b.ActiveListeners) < 0
			}
			return strings.Compare(a.ActiveListeners, b.ActiveListeners) > 0

		default:
			if order == SortDesc {
				return a.ID > b.ID
			}
			return a.ID < b.ID
		}

	})
	t.Refresh()
}

func CreateUserTableObject(PopUpMenu *widget.PopUpMenu) *widget.Table {
	UserTableInit()
	t := widget.NewTableWithHeaders(
		func() (int, int) { return len(UserTableEntries), len(UserHeaders.HeaderNames) },
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
				l.SetText(UserTableEntries[id.Row].Username)
			case 1:
				var admin string
				admin = "False"
				l.Truncation = fyne.TextTruncateOff
				if UserTableEntries[id.Row].Admin == 1 {
					admin = "True"
				}
				l.SetText(admin)
			case 2:
				l.Truncation = fyne.TextTruncateOff
				l.SetText(strconv.Itoa(UserTableEntries[id.Row].ID))
			case 3:
				l.Truncation = fyne.TextTruncateOff
				l.SetText(UserTableEntries[id.Row].ActiveImplants)
			case 4:
				l.Truncation = fyne.TextTruncateOff
				l.SetText(UserTableEntries[id.Row].ActiveListeners)
			}
		},
	)

	t.SetColumnWidth(0, 125)
	t.SetColumnWidth(1, 125)
	t.SetColumnWidth(2, 125)
	t.SetColumnWidth(3, 200)
	t.SetColumnWidth(4, 200)

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
				b.SetText(UserHeaders.HeaderNames[id.Col])
				switch UserSorts[0] {
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			case 1:
				b.SetText(UserHeaders.HeaderNames[id.Col])
				switch UserSorts[1] {
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			case 2:
				b.SetText(UserHeaders.HeaderNames[id.Col])
				switch UserSorts[2] {
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			case 3:
				b.SetText(UserHeaders.HeaderNames[id.Col])
				switch UserSorts[3] {
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			case 4:
				b.SetText(UserHeaders.HeaderNames[id.Col])
				switch UserSorts[4] {
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
				ApplyUserSort(id.Col, t)
			}
			b.Enable()
			b.Refresh()
		}
	}

	return t
}
