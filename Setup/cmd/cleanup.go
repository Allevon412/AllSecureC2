/*
Copyright © 2024 NAME HERE <EMAIL ADDRESS>
*/
package cmd

import (
	"fmt"
	"log"
	"os"
	"path/filepath"
	"strings"

	"github.com/spf13/cobra"
)

type FileInfo struct {
	DataBasePath   string
	ConfigFilePath string
	ProjectDir     string
}

func (FI *FileInfo) FillInfoStruct() {
	var (
		WorkDir string
		content []byte
		err     error
	)
	WorkDir, err = os.Getwd()
	parts := strings.Split(WorkDir, "AllSecure")
	content, err = os.ReadFile(parts[0] + "AllSecure\\Config\\AllSecure.Config")
	if err != nil {
		log.Fatalln("[error] Error opening file", err)
	}
	parts = strings.Split(string(content), "\n")
	for _, line := range parts {
		line_parts := strings.Split(line, "=")
		switch line_parts[0] {
		case "DatabaseFilePath":
			FI.DataBasePath = line_parts[1]
		case "ConfigFilePath":
			FI.ConfigFilePath = line_parts[1]
		case "ProjectDir":
			FI.ProjectDir = line_parts[1]
		default:
			continue
		}
	}
}
func ConfirmDeletion() bool {
	var UserInput string
	fmt.Println("[+++] Are you sure you want to delete AllSecure data? If so type DELETE:")
	_, err := fmt.Scanln(&UserInput)
	if err != nil {
		log.Fatalln("[error] attempting to read user input", err)
	}
	if strings.Compare("DELETE", UserInput) == 0 {
		return true
	}
	return false
}

func (FI *FileInfo) DeleteData(FolderPath string) error {

	err := filepath.Walk(FolderPath, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		if path == FolderPath {
			return nil
		}
		if info.IsDir() {
			return os.RemoveAll(path)
		}
		return os.Remove(path)
	})

	if err == nil {
		err = os.RemoveAll(FolderPath)
	}
	return err
}

// cleanupCmd represents the cleanup command
var cleanupCmd = &cobra.Command{
	Use:   "cleanup",
	Short: "'cleanup' deletes the database and config files",
	Long: `This command is intended to help you automate the destruction of AllSecure data
This could be helpful if there are any issues with your C2 or un-forseen usecases. Then initialize can be used
for resetting the framework.
`,
	Run: func(cmd *cobra.Command, args []string) {
		var FI FileInfo
		FI.FillInfoStruct()
		//TODO production needs to be able to determine current working directory for project folder & go from there.
		if ConfirmDeletion() {
			err := FI.DeleteData(strings.TrimSpace(FI.ProjectDir) + "\\Database")
			if err != nil {
				log.Fatalln("[error] attempting to delete the database path", err)
			}
			err = FI.DeleteData(strings.TrimSpace(FI.ProjectDir) + "\\Config")
			if err != nil {
				log.Fatalln("[error] attempting to delete the config file path", err)
			}
		}
	},
}

func init() {
	rootCmd.AddCommand(cleanupCmd)

	// Here you will define your flags and configuration settings.

	// Cobra supports Persistent Flags which will work for this command
	// and all subcommands, e.g.:
	// cleanupCmd.PersistentFlags().String("foo", "", "A help for foo")

	// Cobra supports local flags which will only run when this command
	// is called directly, e.g.:
	// cleanupCmd.Flags().BoolP("toggle", "t", false, "Help message for toggle")
}
