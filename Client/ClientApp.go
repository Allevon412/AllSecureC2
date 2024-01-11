package main

import (
	"database/sql"
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/layout"
	"fyne.io/fyne/v2/widget"
	_ "github.com/mattn/go-sqlite3"
	"golang.org/x/crypto/bcrypt"
	"log"
	"os"
	"strings"
)

type FileInfo struct {
	DataBasePath   string
	ConfigFilePath string
	ProjectDir     string
}

type User struct {
	ID       int
	Username string
	Password string
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

func (FI *FileInfo) AuthenticateUser(username string, password string) (bool, error) {
	var (
		db       *sql.DB
		err      error
		UserRow  *sql.Rows
		TempUser User
	)
	db, err = sql.Open("sqlite3", FI.DataBasePath)
	if err != nil {
		log.Fatalln("[error] Failed to open database", err)
		return false, err
	}

	if err != nil {
		log.Fatalln("[error] hashing password", err)
		return false, err
	}

	GetUserQuery := `
SELECT id, username, password FROM users WHERE username = ?;
`
	UserRow, err = db.Query(GetUserQuery, username)

	if err != nil {
		log.Fatalln("[error] attempting query", err)
		return false, err
	}
	defer UserRow.Close()

	UserRow.Next()
	err = UserRow.Scan(&TempUser.ID, &TempUser.Username, &TempUser.Password)
	if err != nil {
		return false, err
	}

	err = bcrypt.CompareHashAndPassword([]byte(TempUser.Password), []byte(password))
	if err == nil {
		return true, nil
	}
	return false, nil
}

// TODO: create remote host form.
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

			success, err := FI.AuthenticateUser(UsernameField.Text, PasswordField.Text)
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
//TODO: create team server capability to authetnicate remote clients
