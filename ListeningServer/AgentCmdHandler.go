package ListeningServer

import (
	"AllSecure/ListeningServer/Common"
	"errors"
	"log"
)

func RegisterAgent(data []byte) error {
	var (
		err    error
		ImpCtx Common.ImplantContext
	)

	err = ImpCtx.UnmarshalContext(data)
	if err != nil {
		log.Println("[error] attempting to unmarshal implant data", err)
		return err
	}

	err = SendEvent("RegisterImplant", ImpCtx)
	if err != nil {
		return err
	}

	return nil
}

func QueueCmd() {

}

func SendActiveImplantData() error {
	var (
		err error
	)
	if len(agent_arr) == 0 {
		return errors.New("[info] Agent Array is empty. No agents to send.")
	}
	for _, agent := range agent_arr {
		if agent.Alive {

		}
	}

	return err
}
