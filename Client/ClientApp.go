package main

import (
	"Client/Common"
	"Client/mainmenu"
	"encoding/json"
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/layout"
	"fyne.io/fyne/v2/widget"
	_ "github.com/mattn/go-sqlite3"
	"log"
	"os"
	"strings"
)

type ClientConfig struct {
	Name       string `json:"Name"`
	TSAddress  string `json:"TSAddress"`
	TSPort     string `json:"TSPort"`
	ConfigPath string `json:"ConfigPath"`
	ProjectDir string `json:"ProjectDir"`
}

var (
	CustomUsernameEntry, CustomPasswordEntry, CustomServerFieldEntry *Common.CustomCredentialsEntry
	FailedAuthField                                                  *widget.Label
	myApp                                                            fyne.App
	myWindow                                                         fyne.Window
	ResourcePath                                                     string
	r                                                                fyne.Resource
	err                                                              error
	ClientObj                                                        Common.Client
)

func (CC *ClientConfig) FillInfoStruct() {
	var (
		WorkDir string
		content []byte
	)
	WorkDir, err = os.Getwd()
	parts := strings.Split(WorkDir, "AllSecure")
	content, err = os.ReadFile(parts[0] + "AllSecure\\Config\\Client.Config")
	if err != nil {
		log.Fatalln("[error] Error opening file", err)
	}
	err = json.Unmarshal(content, &CC)
	if err != nil {
		log.Fatalln("[error] Error unmarshalling JSON", err)
	}
}

// TODO remove placeholder info and logic checks for production.
// custom function to authenticate user if the user presses enter on keyboard.
func AuthenticateCredentials() {
	var success bool

	//check to see if default placeholder is used or if an entry is used. then set client obj variable.
	if CustomUsernameEntry.Text == "" && CustomUsernameEntry.PlaceHolder != "" {
		ClientObj.Username = CustomUsernameEntry.PlaceHolder
	} else {
		ClientObj.Username = CustomUsernameEntry.Text
	}
	if CustomServerFieldEntry.Text == "" && CustomServerFieldEntry.PlaceHolder != "" {
		ClientObj.Server = CustomServerFieldEntry.PlaceHolder
	} else {
		ClientObj.Server = CustomServerFieldEntry.Text
	}
	if CustomPasswordEntry.Text == "" && CustomPasswordEntry.PlaceHolder != "" {
		ClientObj.Password = CustomPasswordEntry.PlaceHolder
	} else {
		ClientObj.Password = CustomPasswordEntry.Text
	}

	//attempt authentication
	success, err = Common.AuthenticateUser(&ClientObj)
	if err != nil {
		log.Println("[error] Invalid Credentials, please try again", err)
	}

	//check authentication status
	switch success {
	case true:
		ClientObj.Password = "" // unset the password variable b/c it seems unsafe to store it.
		//go to next screen.
		mainmenu.MainMenu(&ClientObj, myApp, r, ResourcePath)

		myWindow.Close()

	case false:
		FailedAuthField.Hidden = false
	}

}

// AuthenticationForm is used to create the initial form users of the application will authenticate with.
func (CC *ClientConfig) AuthenticationForm() {
	//create new app
	myApp = app.New()
	//create window from app & set title
	myWindow = myApp.NewWindow("AllSecure")

	//create custom username entry field if user presses enter AuthenticateCredentials will run.
	CustomUsernameEntry = Common.NewCustomCredentialEntry(AuthenticateCredentials)
	CustomUsernameEntry.PlaceHolder = "Allevon412" //set place holder for lazy debugger.
	//create password entry field
	CustomPasswordEntry = Common.NewCustomCredentialEntry(AuthenticateCredentials)
	CustomPasswordEntry.PlaceHolder = "Password123"
	CustomPasswordEntry.Password = true
	//create server entry field
	CustomServerFieldEntry = Common.NewCustomCredentialEntry(AuthenticateCredentials)
	CustomServerFieldEntry.PlaceHolder = "127.0.0.1:31337"

	//set ICON
	r, err = fyne.LoadResourceFromPath(CC.ProjectDir + "\\Assets\\server.ico")
	if err != nil {
		log.Println("[error] unable to load server ico file", err)
	}
	myWindow.SetIcon(r)
	//set resource path for future use
	ResourcePath = CC.ProjectDir + "\\Assets\\"

	//failed to auth
	FailedAuthField = widget.NewLabel("Credentials Invalid, Please Try Again")
	FailedAuthField.TextStyle = fyne.TextStyle{Bold: true}
	FailedAuthField.Hidden = true

	//create form
	form := &widget.Form{
		Items: []*widget.FormItem{ // we can specify items in the constructor
			{Text: "Username: ", Widget: CustomUsernameEntry},
			{Text: "Password: ", Widget: CustomPasswordEntry},
			{Text: "Server: ", Widget: CustomServerFieldEntry},
			{Widget: FailedAuthField},
		},
		//when submit button is pressed this is what executes
		OnSubmit: func() { // optional, handle form submission
			AuthenticateCredentials()
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

	//setting background image
	BgImage := canvas.NewImageFromFile(CC.ProjectDir + "\\Assets\\ChipImage.png")
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

	myApp.Settings().SetTheme(&Common.AllSecureTheme{})

	//running window content
	myWindow.ShowAndRun()
}

func main() {
	var CC ClientConfig
	CC.FillInfoStruct()
	CC.AuthenticationForm()
}
