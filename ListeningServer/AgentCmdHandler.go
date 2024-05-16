package ListeningServer

func InitializeAgent(data []byte) error {
	var (
		err error
	)

	err = SendEvent("RegisterImplant", data)
	if err != nil {
		return err
	}

	return nil
}
