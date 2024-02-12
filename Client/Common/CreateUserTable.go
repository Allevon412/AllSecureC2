package Common

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/theme"
	"fyne.io/fyne/v2/widget"
	"sort"
	"strconv"
	"strings"
)

var UserSorts = [3]Direction{}

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
	UserHeaders  UserTableHeaders
	UserRows     []UserData
	TableEntries []UserTableData
	UserTable    *widget.Table
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

	sort.Slice(TableEntries, func(i, j int) bool {
		a := TableEntries[i]
		b := TableEntries[j]

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
	var HeaderNames []string
	HeaderNames = []string{"Username", "Administrator", "ID"}

	t := widget.NewTableWithHeaders(
		func() (int, int) { return len(TableEntries), len(HeaderNames) },
		func() fyne.CanvasObject {
			l := NewCustomUserTableLabel(
				func(e *fyne.PointEvent) {
					PopUpMenu.ShowAtPosition(e.Position)
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
				l.SetText(TableEntries[id.Row].Username)
			case 1:
				var admin string
				admin = "False"
				l.Truncation = fyne.TextTruncateOff
				if TableEntries[id.Row].Admin == 1 {
					admin = "True"
				}
				l.SetText(admin)
			case 2:
				l.Truncation = fyne.TextTruncateOff
				l.SetText(strconv.Itoa(TableEntries[id.Row].ID))
			}
		},
	)

	t.SetColumnWidth(0, 125)
	t.SetColumnWidth(1, 125)
	t.SetColumnWidth(2, 125)

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
				b.SetText(HeaderNames[id.Col])
				switch sorts[id.Col] { // THIS MAY CAUSE ISSUES DUE TO INDEX ERROR
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			case 1:
				b.SetText(HeaderNames[id.Col])
				switch sorts[id.Col] { // THIS MAY CAUSE ISSUES DUE TO INDEX ERROR
				case SortAsc:
					b.Icon = theme.MoveUpIcon()
				case SortDesc:
					b.Icon = theme.MoveDownIcon()
				default:
					b.Icon = nil
				}
			case 2:
				b.SetText(HeaderNames[id.Col])
				switch sorts[id.Col] { // THIS MAY CAUSE ISSUES DUE TO INDEX ERROR
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
