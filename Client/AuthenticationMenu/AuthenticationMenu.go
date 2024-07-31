package AuthenticationMenu

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
	"log"
	"os"
	"strings"
)

var (
	CC                 Common.ClientConfig
	AuthenticationMenu Common.AuthenticationMenu
)

func FillInfoStruct() {
	var (
		WorkDir string
		content []byte
	)
	WorkDir, err := os.Getwd()
	parts := strings.Split(WorkDir, "AllSecure")
	content, err = os.ReadFile(parts[0] + "AllSecure\\Config\\Client.Config")
	if err != nil {
		log.Fatalln("[error] Error opening file", err)
	}
	err = json.Unmarshal(content, &CC)
	if err != nil {
		log.Fatalln("[error] Error unmarshalling JSON", err)
	}
	AuthenticationForm()
}

// TODO remove placeholder info and logic checks for production.
// custom function to authenticate user if the user presses enter on keyboard.
func AuthenticateCredentials() {
	var (
		success bool
		err     error
	)

	//check to see if default placeholder is used or if an entry is used. then set client obj variable.
	if AuthenticationMenu.CustomUsernameEntry.Text == "" && AuthenticationMenu.CustomUsernameEntry.PlaceHolder != "" {
		AuthenticationMenu.ClientObj.Username = AuthenticationMenu.CustomUsernameEntry.PlaceHolder
	} else {
		AuthenticationMenu.ClientObj.Username = AuthenticationMenu.CustomUsernameEntry.Text
	}
	if AuthenticationMenu.CustomServerFieldEntry.Text == "" && AuthenticationMenu.CustomServerFieldEntry.PlaceHolder != "" {
		AuthenticationMenu.ClientObj.Server = AuthenticationMenu.CustomServerFieldEntry.PlaceHolder
	} else {
		AuthenticationMenu.ClientObj.Server = AuthenticationMenu.CustomServerFieldEntry.Text
	}
	if AuthenticationMenu.CustomPasswordEntry.Text == "" && AuthenticationMenu.CustomPasswordEntry.PlaceHolder != "" {
		AuthenticationMenu.ClientObj.Password = AuthenticationMenu.CustomPasswordEntry.PlaceHolder
	} else {
		AuthenticationMenu.ClientObj.Password = AuthenticationMenu.CustomPasswordEntry.Text
	}

	//attempt authentication
	success, err = Common.AuthenticateUser(&AuthenticationMenu.ClientObj)
	if err != nil {
		log.Println("[error] Invalid Credentials, please try again", err)
	}

	//check authentication status
	switch success {
	case true:
		AuthenticationMenu.ClientObj.Authenticated = true
		AuthenticationMenu.ClientObj.Password = "" // unset the password variable b/c it seems unsafe to store it.
		//go to next screen.
		mainmenu.MainMenu(&AuthenticationMenu.ClientObj, AuthenticationMenu.MyApp, AuthenticationMenu.Resource,
			AuthenticationMenu.ResourcePath)

		AuthenticationMenu.AuthenticationWindow.Close()

	case false:
		AuthenticationMenu.FailedAuthField.Hidden = false
	}

}

// AuthenticationForm is used to create the initial form users of the application will authenticate with.
func AuthenticationForm() {
	var (
		err error
	)
	//create new app
	AuthenticationMenu.MyApp = app.New()
	//create window from app & set title
	AuthenticationMenu.AuthenticationWindow = AuthenticationMenu.MyApp.NewWindow("AllSecure")

	//create custom username entry field if user presses enter AuthenticateCredentials will run.
	AuthenticationMenu.CustomUsernameEntry = Common.NewCustomCredentialEntry(AuthenticateCredentials)
	AuthenticationMenu.CustomUsernameEntry.PlaceHolder = "Allevon412" //set place holder for lazy debugger.
	//create password entry field
	AuthenticationMenu.CustomPasswordEntry = Common.NewCustomCredentialEntry(AuthenticateCredentials)
	AuthenticationMenu.CustomPasswordEntry.PlaceHolder = "Password123"
	AuthenticationMenu.CustomPasswordEntry.Password = true
	//create server entry field
	AuthenticationMenu.CustomServerFieldEntry = Common.NewCustomCredentialEntry(AuthenticateCredentials)
	AuthenticationMenu.CustomServerFieldEntry.PlaceHolder = "127.0.0.1:31337"

	//set ICON
	AuthenticationMenu.Resource, err = fyne.LoadResourceFromPath(CC.ProjectDir + "\\Assets\\server.ico")
	if err != nil {
		log.Println("[error] unable to load server ico file", err)
	}
	AuthenticationMenu.AuthenticationWindow.SetIcon(AuthenticationMenu.Resource)
	//set resource path for future use
	AuthenticationMenu.ResourcePath = CC.ProjectDir + "\\Assets\\"

	//failed to auth
	AuthenticationMenu.FailedAuthField = widget.NewLabel("Credentials Invalid, Please Try Again")
	AuthenticationMenu.FailedAuthField.TextStyle = fyne.TextStyle{Bold: true}
	AuthenticationMenu.FailedAuthField.Hidden = true

	//create form
	form := &widget.Form{
		Items: []*widget.FormItem{ // we can specify items in the constructor
			{Text: "Username: ", Widget: AuthenticationMenu.CustomUsernameEntry},
			{Text: "Password: ", Widget: AuthenticationMenu.CustomPasswordEntry},
			{Text: "Server: ", Widget: AuthenticationMenu.CustomServerFieldEntry},
			{Widget: AuthenticationMenu.FailedAuthField},
		},
		//when submit button is pressed this is what executes
		OnSubmit: func() { // optional, handle form submission
			AuthenticateCredentials()
		},
		//when close button is pressed this is what executes
		//cancel button only shows if this is defined
		OnCancel: func() {
			AuthenticationMenu.AuthenticationWindow.Close()
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
	AuthenticationMenu.AuthenticationWindow.SetContent(BgContainer)

	AuthenticationMenu.MyApp.Settings().SetTheme(&Common.AllSecureTheme{})

	//running window content
	AuthenticationMenu.AuthenticationWindow.ShowAndRun()
}
