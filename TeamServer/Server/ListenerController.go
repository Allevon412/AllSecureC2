package Server

import (
	"AllSecure/ListeningServer"
	"AllSecure/TeamServer/Common"
	"AllSecure/TeamServer/Crypt"
	"github.com/gin-gonic/gin"
	"log"
	"strings"
)

func StartListenersInDatabase(list_data []Common.ListenerData, path string) error {
	for _, listener := range list_data {
		// check to make sure the data returned from database is empty. If empty just continue.
		if strings.Compare(listener.ListenerName, "") == 0 {
			continue
		}
		// check to ensure what protocol we're using http or https.
		if strings.Compare(strings.ToLower(listener.Protocol), strings.ToLower("HTTPS")) == 0 {
			//generate cert / key for listening server.
			Cert, Key, err := Crypt.HTTPSGenerateRSACertificate(listener.ListenerName)
			if err != nil {
				log.Fatalln("[error] Failed generating cert / key pair", err)
			}
			//start listener
			err = ListeningServer.Start(listener.HOST, listener.PortBind, true, gin.Default(), Cert, Key, listener.ListenerName, path)
			if err != nil {
				if strings.Compare(err.Error(), "[error] server has already been started") == 0 {
					continue
				} else {
					return err
				}
			}
		} else {
			// start http listener
			err := ListeningServer.Start(listener.HOST, listener.PortBind, false, gin.Default(), []byte{}, []byte{}, listener.ListenerName, path)
			if err != nil {
				if strings.Compare(err.Error(), "[error] server has already been started") == 0 {
					continue
				} else {
					return err
				}
			}
		}

	}
	return nil
}
