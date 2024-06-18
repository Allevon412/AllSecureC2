package mainmenu

import (
	"Client/Common"
	"fyne.io/fyne/v2/widget"
	"log"
	"strings"
)

func CreateImplantTable() *widget.Table {
	return Common.CreateImplantTable()
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

func GetActiveImplants(clientobj *Common.Client) ([]byte, error) {

	endpoint := "https://" + clientobj.Server + "/GetActiveImplants"
	data, err := Common.PerformHTTPReq(clientobj, endpoint, nil)
	if err != nil {
		log.Println("[error] attempting to retrieve user data", err)
	}
	trimedstr := strings.Trim(string(data), "\"")

	decoded := Common.Base64Decode(trimedstr)

	return decoded, nil

}
