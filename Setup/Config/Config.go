package Config

import (
	"fmt"
	"log"
	"os"
)

type TLS struct {
	Cert []byte
	Key  []byte

	CertPath string
	KeyPath  string
}

type ListeningServerConfig struct {
	Name         string
	KillDate     int64
	WorkingHours string
	Method       string
	Port         string
	Secure       bool
	Address      string
	tls          TLS
}

type TeamServerConfig struct {
	Name    string
	Port    string
	Address string
}

type AllSecureConfig struct {
	ProjectDir       string
	DatabaseDir      string
	DatabaseFullPath string
	ConfigDir        string
	ConfigFulLPath   string
	Username         string
	Password         string
	Listener         ListeningServerConfig
	TeamServer       TeamServerConfig
}

func (config *AllSecureConfig) CreateConfigDir() {
	var (
		err error
	)

	//check if configuration folder exists if not create it.
	if !CheckIfFileExists(config.ProjectDir + "\\Server") {
		err = os.Mkdir(config.ProjectDir+"\\Server", 0777)
		config.ConfigDir = config.ProjectDir + "\\Server"
		if err != nil {
			log.Fatalln("[!] Error Creating Server directory", err)
		}
	}

}

func (config *AllSecureConfig) CreateConfigFile() (*os.File, error) {
	var (
		fp  *os.File
		err error
	)
	config.CreateConfigDir()
	// check if the config file exists. if not create it. if it does open a pointer to it.
	if !CheckIfFileExists(config.ConfigDir + "\\AllSecure.Server") {
		fp, err = os.Create(config.ConfigDir + "\\AllSecure.Server")
		if err != nil {
			log.Fatalln("[!] Error Creating AllSecureConfig File", err)
		}
		config.ConfigFulLPath = config.ConfigDir + "\\AllSecure.Server"
		return fp, err
	} else {
		return os.Open(config.ProjectDir + "\\Server" + "\\AllSecure.Server")
	}

}

func (config *AllSecureConfig) WriteDefaultConfig() {

	//set default values
	config.Listener.Name = "AllSecureListener"
	config.Listener.Port = ":443"
	config.Listener.Method = "POST"
	config.Listener.Secure = true
	config.Listener.Address = "127.0.0.1"
	config.Listener.tls.CertPath = ""
	config.Listener.tls.KeyPath = ""

	config.TeamServer.Name = "AllSecureTS"
	config.TeamServer.Port = ":31337"
	config.TeamServer.Address = "127.0.0.1"

	//get handle to config file / create it if it's not been created.
	fp, err := config.CreateConfigFile()
	if err != nil {
		log.Fatalln("[!] Error creating config file", err)
		return
	}
	//close config file at end.
	defer fp.Close()

	//write default values to config file for other components usage.
	_, err = fmt.Fprintf(fp, "ConfigFilePath=%s\n", config.ConfigFulLPath)
	if err != nil {
		log.Fatalln("[error] writing config path to file", err)
		return
	}
	_, err = fmt.Fprintf(fp, "DatabaseFilePath=%s\n", config.DatabaseFullPath)
	if err != nil {
		log.Fatalln("[error] writing database path to file", err)
		return
	}
	_, err = fmt.Fprintf(fp, "ProjectDir=%s\n", config.ProjectDir)
	if err != nil {
		log.Fatalln("[error] writing project path to file", err)
		return
	}
	_, err = fmt.Fprintf(fp, "ListeningServerPort=%s\n", config.Listener.Port)
	if err != nil {
		log.Fatalln("[error] writing ls port to file", err)
		return
	}
	_, err = fmt.Fprintf(fp, "ListeningServerAddress=%s\n", config.Listener.Address)
	if err != nil {
		log.Fatalln("[error] writing ls Address to file", err)
		return
	}
	_, err = fmt.Fprintf(fp, "ListeningServerMethod=%s\n", config.Listener.Method)
	if err != nil {
		log.Fatalln("[error] writing ls method to file", err)
		return
	}
	_, err = fmt.Fprintf(fp, "ListeningServerName=%s\n", config.Listener.Name)
	if err != nil {
		log.Fatalln("[error] writing ls name to file", err)
		return
	}
	switch config.Listener.Secure {
	case true:
		_, err = fmt.Fprintf(fp, "ListeningServerSecure=%s\n", "true")
		if err != nil {
			log.Fatalln("[error] writing ls Secure Value to file", err)
			return
		}
	case false:
		_, err = fmt.Fprintf(fp, "ListeningServerSecure=%s\n", "false")
		if err != nil {
			log.Fatalln("[error] writing ls Secure Value to file", err)
			return
		}
	default:
		_, err = fmt.Fprintf(fp, "ListeningServerSecure=%s\n", "true")
		if err != nil {
			log.Fatalln("[error] writing ls Secure Value to file", err)
			return
		}
	}

	_, err = fmt.Fprintf(fp, "TeamServerName=%s\n", config.TeamServer.Name)
	if err != nil {
		log.Fatalln("[error] writing ts name Value to file", err)
		return
	}
	_, err = fmt.Fprintf(fp, "TeamServerAddress=%s\n", config.TeamServer.Address)
	if err != nil {
		log.Fatalln("[error] writing ts name Value to file", err)
		return
	}
	_, err = fmt.Fprintf(fp, "TeamServerPort=%s\n", config.TeamServer.Port)
	if err != nil {
		log.Fatalln("[error] writing ts name Value to file", err)
		return
	}

}
