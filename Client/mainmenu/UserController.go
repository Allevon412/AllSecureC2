package mainmenu

import (
	"Client/Common"
	"crypto/tls"
	"encoding/json"
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
	"log"
	"net/http"
	"strings"
)

func AddUserSubmitFunc(UsernameEntry, PasswordEntry, AdminEntry *Common.CustomEntry, clientobj *Common.Client, NewWindow fyne.Window) {
	var (
		data      []byte
		admin     bool
		Jdata     []byte
		err       error
		client    http.Client
		transport http.Transport
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

	_, err = Common.PerformHTTPReq(clientobj, endpoint, Jdata)
	if err != nil {
		if err.Error() == "unauthorized" {
			clientobj.Authenticated = false
		}
		log.Println("[error] attempting to retrieve user data", err)
	}

	//obtain the updated list of users.
	data, err = GetUserData(clientobj)
	if err != nil {
		log.Println("[error] attempting to obtain user data", err)
	}
	//unmarshal the list of updated users.
	err = json.Unmarshal(Common.RemoveNullBytes(data), &Common.UserTableEntries)
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
		UsernameEntry, AdminEntry, PasswordEntry *Common.CustomEntry
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

func RemoveUserSubmitFunc(UsernameEntry *Common.CustomEntry, clientobj *Common.Client, NewWindow fyne.Window) {
	var (
		data  []byte
		err   error
		Jdata []byte
	)
	//close the form window at end of the function.
	defer NewWindow.Close()

	endpoint := "https://" + clientobj.Server + "/DeleteUser"

	//marshal the data for http request.
	Jdata, err = json.Marshal(Common.NewUser{
		Username: UsernameEntry.Text,
	})

	_, err = Common.PerformHTTPReq(clientobj, endpoint, Jdata)
	if err != nil {
		if err.Error() == "unauthorized" {
			clientobj.Authenticated = false
		}
		log.Println("[error] attempting to retrieve user data", err)
	}

	//obtain the updated list of users.
	data, err = GetUserData(clientobj)
	if err != nil {
		log.Println("[error] attempting to obtain user data", err)
	}
	//unmarshal the list of updated users.
	err = json.Unmarshal(Common.RemoveNullBytes(data), &Common.UserTableEntries)
	if err != nil {
		log.Println("[error] attempting to unmarshal data", err)
	}
	//refresh the table with the newly added user.
	Common.UserTable.Refresh()
	//exit. user added!
}

func RemoveUser(clientobj *Common.Client, OldWindow fyne.App) error {
	NewWindow := OldWindow.NewWindow("Remove User Form")

	var (
		UsernameEntry *Common.CustomEntry
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

func GetUserData(clientobj *Common.Client) ([]byte, error) {
	endpoint := "https://" + clientobj.Server + "/GetUserData"
	data, err := Common.PerformHTTPReq(clientobj, endpoint, nil)
	if err != nil {
		if err.Error() == "unauthorized" {
			clientobj.Authenticated = false
		}
		log.Println("[error] attempting to retrieve user data", err)
	}

	trimedstr := strings.Trim(string(data), "\"")

	decoded := Common.Base64Decode(trimedstr)

	return decoded, nil
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
	err = json.Unmarshal(Common.RemoveNullBytes(data), &Common.UserTableEntries)
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
	NewWindow.Resize(fyne.NewSize(835, 500))
	NewWindow.Show()
}
