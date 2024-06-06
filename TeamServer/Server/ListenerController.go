package Server

import (
	"AllSecure/ListeningServer"
	"AllSecure/TeamServer/Common"
	"AllSecure/TeamServer/Crypt"
	"github.com/gin-gonic/gin"
	"log"
	"net/http"
	"strconv"
	"strings"
)

func (t *TS) StartListenersInDatabase(list_data []Common.ListenerData) error {
	for _, ListenerData := range list_data {

		var (
			token        string
			TempListener ListeningServer.LS
			err          error
		)

		TempListener.Listener.Config.Address = ListenerData.HOST
		TempListener.Listener.Config.Port = ListenerData.PortBind
		TempListener.Listener.GinEngine = gin.Default()
		TempListener.Listener.Active = true
		TempListener.Listener.Config.Name = ListenerData.ListenerName
		TempListener.Listener.HttpServer = &http.Server{Addr: ListenerData.HOST + ":" + strconv.Itoa(ListenerData.PortBind), Handler: TempListener.Listener.GinEngine}
		TempListener.Listener.TS.Address = t.Server.TSConfig.Address
		TempListener.Listener.TS.Port = t.Server.TSConfig.Port

		//check if our listener is going to use secure comms.
		if strings.Compare(strings.ToLower(ListenerData.Protocol), strings.ToLower("HTTPS")) == 0 {

			//generate a certificate.
			TempListener.Listener.TLS.Cert, TempListener.Listener.TLS.Key, err = Crypt.HTTPSGenerateRSACertificate(ListenerData.HOST)
			if err != nil {
				log.Fatalln("[error] Failed generating cert / key pair", err)
			}
			//set the path for the cert / key.
			TempListener.Listener.TLS.CertPath = t.Server.TSConfig.ProjectDir + "\\ListeningServer\\Assets\\server_" + ListenerData.ListenerName + ".cert"
			TempListener.Listener.TLS.KeyPath = t.Server.TSConfig.ProjectDir + "\\ListeningServer\\Assets\\server_" + ListenerData.ListenerName + ".key"
			TempListener.Listener.Config.Secure = true

			ListeningServers = append(ListeningServers, TempListener) // add newest listener to our list of listening servers.
			token, err = t.CreateToken(9999, ListenerData.ListenerName+"_"+strconv.Itoa(len(ListeningServers)), false)
			if err != nil {
				log.Println("[error] failed to create token")
				return err
			}

			//start server.
			err = TempListener.Start(token)
			if err != nil {
				log.Println("[error] listening server did not start.")
				return err
			}
			//use unsecure comms.
		} else if strings.Compare(strings.ToLower(ListenerData.Protocol), strings.ToLower("HTTP")) == 0 {
			TempListener.Listener.Config.Secure = false

			ListeningServers = append(ListeningServers, TempListener) // add newest listener to our list of listening servers.
			token, err = t.CreateToken(9999, ListenerData.ListenerName+"_"+strconv.Itoa(len(ListeningServers)), false)
			if err != nil {
				log.Println("[error] failed to create token")
				return err
			}

			err = TempListener.Start(token)
			if err != nil {
				log.Println("[error] listening server did not start.")
				return err
			}
		}
	}
	return nil
}
