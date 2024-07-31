package mainmenu

import (
	"Client/Common"
	"encoding/json"
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/widget"
	"log"
	"strings"
)

func SendImplantCommand(Command string) {

}

func InteractWithImplant(FyneApp fyne.App) {
	//Get the selected implant number.
	ImplantNum := Common.ImplantData[Common.SelectedImplant.Row].ImplantNum
	log.Println("Interacting with implant number: ", ImplantNum)
	NewWindow := FyneApp.NewWindow("Interact with Implant")

	//Create the form for interacting with the implant.
	implantLog := widget.NewMultiLineEntry()
	implantLog.Disable()
	implantLog.SetMinRowsVisible(30)

	//some fucking go wizardry to ensure that we can update our team's chat if we press enter with text in the chat entry field.
	customEntryWidget = Common.NewCustomChatEntry(SendImplantCommand) //TODO FIX

	//Teams chat entry's combined into one form.
	ImplantForm := &widget.Form{
		Items: []*widget.FormItem{
			{Text: "", Widget: implantLog},
			{Text: Common.ImplantData[Common.SelectedImplant.Row].ImplantName, Widget: customEntryWidget},
		},
		OnSubmit: func() {
			//TODO FIX.
		},
		OnCancel:   func() {}, //TODO FIX.
		SubmitText: "",
		CancelText: "Clear Chat History",
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
