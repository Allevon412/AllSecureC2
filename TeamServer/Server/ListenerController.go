package Server

import (
	"AllSecure/ListeningServer"
	Common2 "AllSecure/ListeningServer/Common"
	"AllSecure/TeamServer/Common"
	"AllSecure/TeamServer/Crypt"
	"github.com/gin-gonic/gin"
	"log"
	"strings"
)

func StartListenersInDatabase(list_data []Common.ListenerData, path string, jwttoken, TSAddr string, TSPort string) error {
	for _, listener := range list_data {
		var err error
		var NewListener Common2.NewListener
		NewListener.ListenerName = listener.ListenerName
		NewListener.Address = listener.HOST
		NewListener.TSAddr = TSAddr
		NewListener.Jwttoken = jwttoken
		NewListener.Engine = gin.Default()
		NewListener.Port = listener.PortBind
		NewListener.Path = path
		NewListener.TSPort = TSPort

		// check to make sure the data returned from database is empty. If empty just continue.
		if strings.Compare(listener.ListenerName, "") == 0 {
			continue
		}
		// check to ensure what protocol we're using http or https.
		if strings.Compare(strings.ToLower(listener.Protocol), strings.ToLower("HTTPS")) == 0 {
			NewListener.Secure = true

			//generate cert / key for listening server.
			NewListener.Cert, NewListener.Key, err = Crypt.HTTPSGenerateRSACertificate(listener.ListenerName)
			if err != nil {
				log.Fatalln("[error] Failed generating cert / key pair", err)
			}

			//start listener
			err = ListeningServer.Start(NewListener)
			if err != nil {
				if strings.Compare(err.Error(), "[error] server has already been started") == 0 {
					continue
				} else {
					return err
				}
			}
		} else {
			NewListener.Secure = false
			// start http listener
			err = ListeningServer.Start(NewListener)
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
