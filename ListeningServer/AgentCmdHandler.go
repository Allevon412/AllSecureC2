package ListeningServer

import (
	"AllSecure/ListeningServer/Common"
	"log"
)

func InitializeAgent(data []byte) error {
	var (
		err    error
		ImpCtx Common.ImplantContext
	)

	err = ImpCtx.UnmarshalBinary(data)
	if err != nil {
		log.Println("[error] attempting to unmarshal implant data", err)
		return err
	}

	err = SendEvent("RegisterImplant", ImpCtx)
	if err != nil {
		return err
	}

	agent_arr = append(agent_arr, ImpCtx)

	return nil
}
