package ListeningServer

func InitializeAgent(data []byte) error {
	var (
		err error
	)

	err = SendEvent("NewImplant", data)
	if err != nil {
		return err
	}

	return nil
}
