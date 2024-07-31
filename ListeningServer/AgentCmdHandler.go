package ListeningServer

import (
	"AllSecure/ListeningServer/Common"
)

func RegisterAgent(Agent *Common.Implant, data []byte) error {
	var (
		err error
	)

	Agent.Context.UnmarshalContext(data)

	err = SendEvent("RegisterImplant", Agent.Context)
	if err != nil {
		return err
	}

	return nil
}
