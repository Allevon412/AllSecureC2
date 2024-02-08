package mainmenu

import (
	"Client/Common"
	"crypto/tls"
	"encoding/base64"
	"encoding/json"
	"fmt"
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/theme"
	"fyne.io/fyne/v2/widget"
	"io"
	"log"
	"net/http"
	"sort"
	"strconv"
	"strings"
)

type Direction int

const (
	SortOff Direction = iota
	SortAsc
	SortDesc
)

var sorts = [3]Direction{}

// TODO add the add user functionality
func AddUser() {

}

// TODO add the remove user functionality
func RemoveUser() {

}

func base64decode(encodedstr string) []byte {
	dst := make([]byte, base64.StdEncoding.DecodedLen(len(encodedstr)))
	_, err := base64.StdEncoding.Decode(dst, []byte(encodedstr))
	if err != nil {
		fmt.Println("decode error:", err)
		return []byte{}
	}
	return dst
}

// TODO create fetch user data function. to retrieve user data table bindings.
func GetUserData(clientobj *Common.Client) ([]byte, error) {
	var (
		token     []byte
		err       error
		req       *http.Request
		resp      *http.Response
		transport http.Transport
		client    http.Client
		data      []byte
		decoded   []byte
	)

	//setup the http tls configuration
	transport.TLSClientConfig = &tls.Config{InsecureSkipVerify: true}
	client.Transport = &transport
	//setup our endpoint
	endpoint := "https://" + clientobj.Server + "/GetUserData"

	//create a new request that our client will perform.
	req, err = http.NewRequest("POST", endpoint, nil)
	if err != nil {
		log.Println("[error] attempting to generate a new HTTP request with the specified parameters", err)
	}
	//marshal token into it's byte form.
	token, err = json.Marshal(clientobj.Cookie.Token.JwtToken)
	//set our request authorization header to be our jwt  token.
	req.Header.Set("Authorization", strings.Trim(string(token), "\""))

	//perform the request.
	resp, err = client.Do(req)
	if err != nil {
		log.Println("[error] attempting to perform request", err)
		return nil, err
	}

	defer resp.Body.Close()
	status := resp.StatusCode
	if status != http.StatusOK {
		log.Println("[error] in attempting to get user data. status: ", status)
		return nil, nil
	}

	data, err = io.ReadAll(resp.Body)
	if err != nil {
		log.Println("[error] attempting to read resp body")
		return nil, err
	}
	trimedstr := strings.Trim(string(data), "\"")

	decoded = base64decode(trimedstr)

	return decoded, nil
}

func ApplySort(col int, t *widget.Table, TableEntries []Common.UserTableData) {
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

// TODO: CREATE EXTENDED LABEL WIDGET AND THEN CREATE TAPPEDSECONDARY HANDLER
// TODO: SHOW MENU IN TAPPED SECONDARY HANDLER
// TODO: MOVE THIS CODE TO COMMON SECTION AND CALL IT FROM THAT PACKAGE.
func CreateUserTableObject(TableEntries []Common.UserTableData) *widget.Table {
	var HeaderNames []string
	HeaderNames = []string{"Username", "Administrator", "ID"}
	t := widget.NewTableWithHeaders(func() (int, int) { return len(TableEntries), len(HeaderNames) },
		func() fyne.CanvasObject { l := widget.NewLabel(""); return l },
		func(id widget.TableCellID, o fyne.CanvasObject) {
			l := o.(*widget.Label)
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
				ApplySort(id.Col, t, TableEntries)
			}
			b.Enable()
			b.Refresh()
		}
	}

	return t
}

func removeNullBytes(data []byte) []byte {
	var result []byte

	for _, b := range data {
		if b != 0x00 {
			result = append(result, b)
		}
	}

	return result
}

func CreateUserWindow(clientobj *Common.Client, OldWindow fyne.App) {
	var (
		data         []byte
		TableEntries []Common.UserTableData
		SingleEntry  Common.UserTableData
	)
	NewWindow := OldWindow.NewWindow("Admin Controls")

	data, err := GetUserData(clientobj)
	if err != nil {
		log.Println("[error] attempting to obtain user data", err)
	}
	if data[0] == 123 {
		err = json.Unmarshal(removeNullBytes(data), &SingleEntry)
		if err != nil {
			log.Println("[error] attempting to unmarshal data", err)
		}
		TableEntries = append(TableEntries, SingleEntry)
	} else {
		err = json.Unmarshal(removeNullBytes(data), &TableEntries)
		if err != nil {
			log.Println("[error] attempting to unmarshal data", err)
		}
	}

	UserTable := CreateUserTableObject(TableEntries)
	NewWindow.SetContent(UserTable)
	NewWindow.Resize(fyne.NewSize(500, 500))
	NewWindow.Show()
}
