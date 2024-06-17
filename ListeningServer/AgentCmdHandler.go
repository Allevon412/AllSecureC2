package ListeningServer

import (
	"AllSecure/ListeningServer/Common"
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
