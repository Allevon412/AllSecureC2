package ListeningServer

import (
	"AllSecure/ListeningServer/Common"
	"log"
)

func RegisterAgent(Agent *Common.Implant, data []byte) error {
	var (
		err error
	)

	Agent.Context.UnmarshalContext(data)
	if err != nil {
		log.Println("[error] attempting to unmarshal implant data", err)
		return err
	}

	err = SendEvent("RegisterImplant", Agent.Context)
	if err != nil {
		return err
	}

	return nil
}

func QueueCmd() {

}
