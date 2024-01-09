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

func (config *AllSecureConfig) CheckIfDatabaseFolderExists() bool {

	//check if database exists
	if config.DatabaseDir == "" {
		log.Fatalln("[!] There is no Database Directory in config")
		return false
	}
	_, err := os.Stat(config.DatabaseDir)
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
	//check if database file exists
	if config.CheckIfDatabaseFolderExists() {
		config.DatabaseFullPath = config.DatabaseDir + "\\AllSecure.db"
		return os.Open(config.DatabaseDir + "\\AllSecure.db")

	}
	err := config.CreateDatabaseFolder()
	if err != nil {
		log.Fatalln(err)
		return nil, err
	}
	return config.CreateDatabase()

}
