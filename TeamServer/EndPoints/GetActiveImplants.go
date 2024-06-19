package EndPoints

import (
	"AllSecure/TeamServer/Common/Types"
)

func GetActiveImplants(client *Types.Client, Message Types.WebSocketMessage) error {
	//create get active implants messsage type
	var (
		err error
	)

	err = client.Conn.WriteJSON(Message)
	if err != nil {
		return err
	}

	return nil
}
