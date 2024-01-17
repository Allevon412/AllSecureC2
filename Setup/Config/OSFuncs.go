package Config

import (
	"errors"
	"log"
	"os"
)

func CheckIfFileExists(FilePath string) bool {
	if FilePath == "" {
		log.Fatalln("[!] No path given")
		return false
	}
	_, err := os.Stat(FilePath)
	if errors.Is(err, os.ErrNotExist) {
		return false
	}

	return true
}

func (config *AllSecureConfig) CheckIfExists(path string) bool {

	//check if database exists
	if path == "" {
		log.Fatalln("[!] There is no Database Directory in config")
		return false
	}
	_, err := os.Stat(path)
	if errors.Is(err, os.ErrNotExist) {
		return false
	}

	return true
}

func (config *AllSecureConfig) CreateDatabaseFolder() error {
	if config.DatabaseDir == "" {
		log.Fatalln("[!] There is no Database Directory in config")
		return os.ErrNotExist
	}
	err := os.Mkdir(config.DatabaseDir, 0777)
	if err != nil {
		return err
	}
	config.DatabaseDir = config.ProjectDir + "\\Database"
	return nil
}

func (config *AllSecureConfig) CreateDatabase() (*os.File, error) {
	if config.DatabaseDir == "" {
		log.Fatalln("[!] There is no Database Directory in config")
		return nil, os.ErrNotExist
	}
	fp, err := os.Create(config.DatabaseDir + "\\AllSecure.db")
	if err != nil {
		return nil, err
	}
	config.DatabaseFullPath = config.DatabaseDir + "\\AllSecure.db"
	return fp, nil
}

func (config *AllSecureConfig) CheckForAndCreateDatabase() (*os.File, error) {
	//check if database folder exists
	if config.CheckIfExists(config.DatabaseDir) {
		config.DatabaseFullPath = config.DatabaseDir + "\\AllSecure.db"
		//check if database file exists
		if config.CheckIfExists(config.DatabaseDir + "\\AllSecure.db") {
			//if so open it.
			return os.Open(config.DatabaseDir + "\\AllSecure.db")
		} else {
			//if not create database & return handle to it.
			return config.CreateDatabase()
		}

	}
	//create folder since it doesn't exist.
	err := config.CreateDatabaseFolder()
	if err != nil {
		log.Fatalln(err)
		return nil, err
	}
	//create database since it cannot logically exist w/o the folder
	return config.CreateDatabase()

}
