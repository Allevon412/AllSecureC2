package mainmenu

import (
	"Client/Common"
	"bytes"
	"crypto/tls"
	"encoding/base64"
	"encoding/json"
	"fmt"
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
	"io"
	"log"
	"net/http"
	"strings"
)

// TODO add the add user functionality
func AddUser(clientobj *Common.Client, OldWindow fyne.App) error {

	NewWindow := OldWindow.NewWindow("Add User Form")

	var (
		token                                    []byte
		err                                      error
		req                                      *http.Request
		resp                                     *http.Response
		transport                                http.Transport
		client                                   http.Client
		admin                                    bool
		Jdata                                    []byte
		UsernameEntry, AdminEntry, PasswordEntry widget.Entry
	)
	PasswordEntry.Password = true

	form := &widget.Form{
		Items: []*widget.FormItem{
			{
				Text:     "Username: ",
				Widget:   &UsernameEntry,
				HintText: "Allevon412",
			},
			{
				Text:     "Password: ",
				Widget:   &PasswordEntry,
				HintText: "P@ssw0rd123!",
			},
			{
				Text:     "Administrator: ",
				Widget:   &AdminEntry,
				HintText: "True",
			},
		},
		OnSubmit: func() {
			//setup the http tls configuration
			transport.TLSClientConfig = &tls.Config{InsecureSkipVerify: true}
			client.Transport = &transport
			//setup our endpoint
			endpoint := "https://" + clientobj.Server + "/AddNewUser"

			admin, err = Common.ParseBool(AdminEntry.Text)
			Jdata, err = json.Marshal(Common.NewUser{
				Username: UsernameEntry.Text,
				Password: PasswordEntry.Text,
				Admin:    admin,
			})

			//create a new request that our client will perform.
			req, err = http.NewRequest("POST", endpoint, bytes.NewBuffer(Jdata))
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

			}

			defer resp.Body.Close()
			status := resp.StatusCode
			if status != http.StatusOK {
				log.Println("[error] in attempting to get user data. status: ", status)

			}
			NewWindow.Close()
		},

		OnCancel:   NewWindow.Close,
		SubmitText: "Create User",
		CancelText: "Exit",
	}

	stack := container.NewStack(form)
	NewWindow.SetContent(stack)
	NewWindow.Resize(fyne.NewSize(700, 225))
	NewWindow.Show()
	return nil
}

// TODO add the remove user functionality
func RemoveUser() {

}

// TODO add the edit user functionality
func EditUser() {

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
	)
	NewWindow := OldWindow.NewWindow("Admin Controls")

	data, err := GetUserData(clientobj)
	if err != nil {
		log.Println("[error] attempting to obtain user data", err)
	}
	log.Println(string(data))
	err = json.Unmarshal(removeNullBytes(data), &TableEntries)
	if err != nil {
		log.Println("[error] attempting to unmarshal data", err)
	}
	//create menu items for popupmenu
	add := fyne.NewMenuItem("Add new user", func() { AddUser(clientobj, OldWindow) })
	edit := fyne.NewMenuItem("Edit this user", EditUser)
	del := fyne.NewMenuItem("Delete user", RemoveUser)
	//create a new menu from menu items
	menu := fyne.NewMenu("Options", add, edit, del)
	//create popupmenu from menu.
	PopUpMenu := widget.NewPopUpMenu(menu, NewWindow.Canvas())
	//pass the table entry, and popup menu to our create table options.
	UserTable := Common.CreateUserTableObject(TableEntries, PopUpMenu)
	NewWindow.SetContent(UserTable)
	NewWindow.Resize(fyne.NewSize(500, 500))
	NewWindow.Show()
}
