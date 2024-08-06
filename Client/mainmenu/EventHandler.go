package mainmenu

import (
	"Client/Common"
	"encoding/json"
	"github.com/gorilla/websocket"
	"log"
)

func CheckForNewEventsFromWS() {

	for {
		var NewWSMessage Common.WebSocketMessage
		err := g_clientobj.Conn.ReadJSON(&NewWSMessage)
		if err != nil {
			if websocket.IsUnexpectedCloseError(err, websocket.CloseGoingAway, websocket.CloseAbnormalClosure) {
				log.Printf("WebSocket closed unexpectedly: %v", err)
				break
			} else if websocket.IsCloseError(err, websocket.CloseNormalClosure, websocket.CloseGoingAway) {
				log.Printf("WebSocket closed normally: %v", err)
				break
			} else {
				log.Printf("[error] reading WebSocket message: %v", err)
				break
			}
		}
		switch NewWSMessage.MessageType {
		case "ChatMessage":
			UpdateChat(NewWSMessage.Message)
			break
		case "RegisterImplant":
			var TempImplantData Common.ImplantTableData
			var Reregisted = false

			err = json.Unmarshal([]byte(NewWSMessage.Message), &TempImplantData)
			if err != nil {
				log.Println("[error] attempting to read implant data from web socket message", err)
				continue
			}
			//TODO add support for maps instead of slices of our user / implant / listener data structures.
			for i, implant := range Common.ImplantData {
				if implant.ImplantName == TempImplantData.ImplantName {
					ReregisterImplant(TempImplantData, i)
					Reregisted = true
					break
				}
			}

			if Reregisted {
				break
			}

			AddImplantToTable(TempImplantData)
			break

		case "UpdateHealth":
			var TempImplantData Common.ImplantTableData
			err = json.Unmarshal([]byte(NewWSMessage.Message), &TempImplantData)
			if err != nil {
				log.Println("[error] attempting to read implant data from web socket message", err)
				break
			}
			UpdateImplantHealth(TempImplantData)
			break

		case "UpdateCheckin":
			var TempImplantData Common.ImplantTableData
			err = json.Unmarshal([]byte(NewWSMessage.Message), &TempImplantData)
			if err != nil {
				log.Println("[error] attempting to read implant data from web socket message", err)
				break
			}
			UpdateImplantCheckin(TempImplantData)
			break

		case "SendModuleData":
			var (
				TempImplantData       map[string]interface{}
				TempModuleData        map[string]interface{}
				ImplantTableData      Common.ImplantTableData
				ModuleDataMap         map[string][]byte
				implantTableDataBytes []byte
				moduleDataBytes       []byte
				ok                    bool
			)
			data := map[string]interface{}{"ImplantData": Common.ImplantTableData{}, "ModuleData": map[string][]byte{}}
			log.Println("Received module data")
			log.Println(NewWSMessage.Message)
			err = json.Unmarshal([]byte(NewWSMessage.Message), &data)
			if err != nil {
				log.Println("[error] attempting to read implant data from web socket message", err)
				break
			}

			log.Println(data)
			TempImplantData, ok = data["ImplantData"].(map[string]interface{})
			if !ok {
				log.Println("[error] attempting to cast implant data to map[string]interface{}", err)
				break
			}
			TempModuleData, ok = data["ModuleData"].(map[string]interface{})
			if !ok {
				log.Println("[error] attempting to cast module data to map[string]interface{}", err)
				break
			}

			implantTableDataBytes, err = json.Marshal(TempImplantData)
			if err != nil {
				log.Println("[error] attempting to marshal the implant data", err)
				break
			}
			err = json.Unmarshal(implantTableDataBytes, &ImplantTableData)
			if err != nil {
				log.Println("[error] attempting to unmarshal the implant data", err)
				break
			}
			moduleDataBytes, err = json.Marshal(TempModuleData)
			if err != nil {
				log.Println("[error] attempting to marshal the module data", err)
				break
			}
			err = json.Unmarshal(moduleDataBytes, &ModuleDataMap)
			if err != nil {
				log.Println("[error] attempting to unmarshal the module data", err)
				break
			}

			SendDataToImplantWindow(ImplantTableData, ModuleDataMap, 1)
			break

		default:
			break
		}
	} // our websocket has closed. Attempt to reconnect.
	err := g_clientobj.Conn.Close() // since we can no longer read messages close our client and try to open a new one.
	if err != nil {
		log.Println("[error] attempting to close websocket connection", err)
	}
	g_clientobj.Authenticated = false
}
