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

func AddUserSubmitFunc(UsernameEntry, PasswordEntry, AdminEntry *Common.CustomCredentialsEntry, clientobj *Common.Client, NewWindow fyne.Window) {
	var (
		data      []byte
		admin     bool
		Jdata     []byte
		err       error
		token     []byte
		client    http.Client
		transport http.Transport
		resp      *http.Response
		req       *http.Request
	)
	//close the form window at end of the function.
	defer NewWindow.Close()

	//setup the http tls configuration
	transport.TLSClientConfig = &tls.Config{InsecureSkipVerify: true}
	client.Transport = &transport
	//setup our endpoint
	endpoint := "https://" + clientobj.Server + "/AddNewUser"

	//parse our user bool input to bind it to our NewUser struct.
	admin, err = Common.ParseBool(AdminEntry.Text)
	//marshal the data for http request.
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
	//close http body @ end.
	defer resp.Body.Close()

	//check http response code.
	status := resp.StatusCode
	if status != http.StatusOK {
		log.Println("[error] in attempting to get user data. status: ", status)
	}

	//obtain the updated list of users.
	data, err = GetUserData(clientobj)
	if err != nil {
		log.Println("[error] attempting to obtain user data", err)
	}
	//unmarshal the list of updated users.
	err = json.Unmarshal(removeNullBytes(data), &Common.TableEntries)
	if err != nil {
		log.Println("[error] attempting to unmarshal data", err)
	}
	//refresh the table with the newly added user.
	Common.UserTable.Refresh()
	//exit. user added!
}

func AddUser(clientobj *Common.Client, OldWindow fyne.App) error {

	NewWindow := OldWindow.NewWindow("Add User Form")

	var (
		UsernameEntry, AdminEntry, PasswordEntry *Common.CustomCredentialsEntry
	)
	UsernameEntry = Common.NewCustomCredentialEntry(func() { AddUserSubmitFunc(UsernameEntry, PasswordEntry, AdminEntry, clientobj, NewWindow) })
	PasswordEntry = Common.NewCustomCredentialEntry(func() { AddUserSubmitFunc(UsernameEntry, PasswordEntry, AdminEntry, clientobj, NewWindow) })
	AdminEntry = Common.NewCustomCredentialEntry(func() { AddUserSubmitFunc(UsernameEntry, PasswordEntry, AdminEntry, clientobj, NewWindow) })
	PasswordEntry.Password = true

	//create a new form.
	form := &widget.Form{
		Items: []*widget.FormItem{
			{
				Text:     "Username: ",
				Widget:   UsernameEntry,
				HintText: "Allevon412",
			},
			{
				Text:     "Password: ",
				Widget:   PasswordEntry,
				HintText: "P@ssw0rd123!",
			},
			{
				Text:     "Administrator: ",
				Widget:   AdminEntry,
				HintText: "True",
			},
		},
		OnSubmit: func() { AddUserSubmitFunc(UsernameEntry, PasswordEntry, AdminEntry, clientobj, NewWindow) },

		OnCancel:   NewWindow.Close,
		SubmitText: "Create User",
		CancelText: "Exit",
	}

	//show our form in the new window.
	stack := container.NewStack(form)
	NewWindow.SetContent(stack)
	NewWindow.Resize(fyne.NewSize(700, 225))
	NewWindow.Show()
	return nil
}

func RemoveUserSubmitFunc(UsernameEntry *Common.CustomCredentialsEntry, clientobj *Common.Client, NewWindow fyne.Window) {
	var (
		data      []byte
		token     []byte
		err       error
		req       *http.Request
		resp      *http.Response
		transport http.Transport
		client    http.Client
		Jdata     []byte
	)
	//close the form window at end of the function.
	defer NewWindow.Close()

	//setup the http tls configuration
	transport.TLSClientConfig = &tls.Config{InsecureSkipVerify: true}
	client.Transport = &transport
	//setup our endpoint
	endpoint := "https://" + clientobj.Server + "/DeleteUser"

	//marshal the data for http request.
	Jdata, err = json.Marshal(Common.NewUser{
		Username: UsernameEntry.Text,
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
	//close http body @ end.
	defer resp.Body.Close()

	//check http response code.
	status := resp.StatusCode
	if status != http.StatusOK {
		log.Println("[error] in attempting to get user data. status: ", status)
	}

	//obtain the updated list of users.
	data, err = GetUserData(clientobj)
	if err != nil {
		log.Println("[error] attempting to obtain user data", err)
	}
	//unmarshal the list of updated users.
	err = json.Unmarshal(removeNullBytes(data), &Common.TableEntries)
	if err != nil {
		log.Println("[error] attempting to unmarshal data", err)
	}
	//refresh the table with the newly added user.
	Common.UserTable.Refresh()
	//exit. user added!
}

func RemoveUser(clientobj *Common.Client, OldWindow fyne.App) error {
	NewWindow := OldWindow.NewWindow("Add User Form")

	var (
		UsernameEntry *Common.CustomCredentialsEntry
	)

	UsernameEntry = Common.NewCustomCredentialEntry(func() { RemoveUserSubmitFunc(UsernameEntry, clientobj, NewWindow) })
	//create a new form.
	form := &widget.Form{
		Items: []*widget.FormItem{
			{
				Text:     "User to Delete: ",
				Widget:   UsernameEntry,
				HintText: "Allevon412",
			},
		},
		OnSubmit: func() { RemoveUserSubmitFunc(UsernameEntry, clientobj, NewWindow) },

		OnCancel:   NewWindow.Close,
		SubmitText: "Create User",
		CancelText: "Exit",
	}

	//show our form in the new window.
	stack := container.NewStack(form)
	NewWindow.SetContent(stack)
	NewWindow.Resize(fyne.NewSize(700, 150))
	NewWindow.Show()
	return nil
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
		data []byte
	)
	//create new admin control window
	NewWindow := OldWindow.NewWindow("Admin Controls")
	//obtain list of users from remote server database.
	data, err := GetUserData(clientobj)
	if err != nil {
		log.Println("[error] attempting to obtain user data", err)
	}
	//unmarshal the data and store it.
	err = json.Unmarshal(removeNullBytes(data), &Common.TableEntries)
	if err != nil {
		log.Println("[error] attempting to unmarshal data", err)
	}

	//create menu items for popupmenu
	add := fyne.NewMenuItem("Add new user", func() {
		//create new user form when Add new user is selected from menu
		err = AddUser(clientobj, OldWindow)
		if err != nil {
			log.Println("[error] attempting to add user", err)
		}
	})

	//delete user popup menu
	del := fyne.NewMenuItem("Delete user", func() {
		err = RemoveUser(clientobj, OldWindow)
		if err != nil {
			log.Println("[error] attempting to add user", err)
		}
	})

	//create a new menu from menu items
	menu := fyne.NewMenu("Options", add, del)
	//create popupmenu from menu.
	PopUpMenu := widget.NewPopUpMenu(menu, NewWindow.Canvas())
	//pass the table entry, and popup menu to our create table options.
	Common.UserTable = Common.CreateUserTableObject(PopUpMenu)
	//create user table window and show it.
	NewWindow.SetContent(Common.UserTable)
	NewWindow.Resize(fyne.NewSize(500, 500))
	NewWindow.Show()
}
