package mainmenu

import (
	"Client/Common"
	"fyne.io/fyne/v2/widget"
)

func CreateImplantTable() *widget.Table {
	return Common.CreateImplantTable()
}

func AddImplantToTable(ImplantData Common.ImplantTableData) {
	count := len(Common.ImplantData)
	ImplantData.ImplantNum = count + 1
	Common.ImplantData = append(Common.ImplantData, ImplantData)
	Common.ImplantTable.Refresh()
}

func RemoveImplantFromTable() {

}
