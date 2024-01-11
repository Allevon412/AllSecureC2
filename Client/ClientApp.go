package main

import (
	"Client/Common"
	"bytes"
	"crypto/tls"
	"encoding/json"
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/layout"
	"fyne.io/fyne/v2/widget"
	_ "github.com/mattn/go-sqlite3"
	"log"
	"net/http"
	"os"
	"strings"
)

type FileInfo struct {
	DataBasePath   string
	ConfigFilePath string
	ProjectDir     string
}

func (FI *FileInfo) FillInfoStruct() {
	var (
		WorkDir string
		content []byte
		err     error
	)
	WorkDir, err = os.Getwd()
	parts := strings.Split(WorkDir, "AllSecure")
	content, err = os.ReadFile(parts[0] + "AllSecure\\Config\\AllSecure.Config")
	if err != nil {
		log.Fatalln("[error] Error opening file", err)
	}
	parts = strings.Split(string(content), "\n")
	for _, line := range parts {
		line_parts := strings.Split(line, "=")
		switch line_parts[0] {
		case "DatabaseFilePath":
			FI.DataBasePath = line_parts[1]
		case "ConfigFilePath":
			FI.ConfigFilePath = line_parts[1]
		case "ProjectDir":
			FI.ProjectDir = line_parts[1]
		default:
			continue
		}
	}
}

func AuthenticateUser(username, password, server string) (bool, error) {

	var (
		User      Common.User
		Jdata     []byte
		err       error
		resp      *http.Response
		transport http.Transport
		client    http.Client
	)
	transport.TLSClientConfig = &tls.Config{InsecureSkipVerify: true}
	client.Transport = &transport

	User.Username = username
	User.Password = password

	Jdata, err = json.Marshal(User)
	if err != nil {
		log.Println("[error] attempting to marshal json data", err)
		return false, err
	}
	endpoint := "https://" + server + "/AuthenticateUser"

	resp, err = client.Post(endpoint, "application/json", bytes.NewBuffer(Jdata))
	if err != nil {
		log.Println("[error] attmepting post request", err)
		return false, err
	}

	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		log.Println("[error] invalid credentials please try again")
		return false, nil
	}

	return true, nil
}

// AuthenticationForm is used to create the initial form users of the application will authenticate with.
func (FI *FileInfo) AuthenticationForm() {

	//create new app
	myApp := app.New()
	//create window from app & set title
	myWindow := myApp.NewWindow("AllSecure")

	//create username entry field
	UsernameField := widget.NewEntry()
	//create password entry field
	PasswordField := widget.NewPasswordEntry()
	//no auth
	FailedAuthField := widget.NewLabel("Credentials Invalid, Please Try Again")
	FailedAuthField.TextStyle = fyne.TextStyle{Bold: true}
	FailedAuthField.Hidden = true
	ServerField := widget.NewEntry()
	ServerField.PlaceHolder = "127.0.0.1:31337"

	//create form
	form := &widget.Form{
		Items: []*widget.FormItem{ // we can specify items in the constructor
			{Text: "Username: ", Widget: UsernameField},
			{Text: "Password: ", Widget: PasswordField},
			{Text: "Server: ", Widget: ServerField},
			{Widget: FailedAuthField},
		},
		//when submit button is pressed this is what executes
		OnSubmit: func() { // optional, handle form submission
			var success bool
			var err error

			//check if our server field is the default setting if so use it.
			if ServerField.Text == "" {
				success, err = AuthenticateUser(UsernameField.Text, PasswordField.Text, ServerField.PlaceHolder)
			} else {
				success, err = AuthenticateUser(UsernameField.Text, PasswordField.Text, ServerField.Text)
			}
			if err != nil {
				log.Println("[error] Invalid Credentials, please try again", err)
			}
			switch success {
			case true:
				myWindow.Close()
			case false:
				FailedAuthField.Hidden = false
			}

		},
		//when close button is pressed this is what executes
		//cancel button only shows if this is defined
		OnCancel: func() {
			myWindow.Close()
		},
		//submit / cancel text replacements
		SubmitText: "Authenticate",
		CancelText: "Exit AllSecure",
	}

	//set ICON
	r, err := fyne.LoadResourceFromPath(FI.ProjectDir + "\\Assets\\server.ico")
	if err != nil {
		log.Println("[error] unable to load server ico file", err)
	}
	myWindow.SetIcon(r)

	//setting background image
	BgImage := canvas.NewImageFromFile(FI.ProjectDir + "\\Assets\\ChipImage.png")
	//attempting to fill background with image.
	BgImage.FillMode = canvas.ImageFillOriginal

	//centering the form
	CenteredForm := container.New(layout.NewCenterLayout(), form)
	//setting background of form
	BgContainer := container.NewStack(BgImage, CenteredForm)
	//setting size of form
	//myWindow.Resize(fyne.NewSize(500, 500))
	//setting window content
	myWindow.SetContent(BgContainer)
	//running window content
	myWindow.ShowAndRun()
}

func main() {
	var FI FileInfo
	FI.FillInfoStruct()
	FI.AuthenticationForm()
	//boarder()
}

//TODO: create the main menu after authentication.
