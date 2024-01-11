/*
Copyright © 2024 NAME HERE <EMAIL ADDRESS>
*/
package cmd

import (
	"Setup/Config"
	"fmt"
	"github.com/spf13/cobra"
	"log"
)

var config Config.AllSecureConfig

// initializeCmd represents the initialize command
// TODO: create usage of this command to parse a config file.
var initializeCmd = &cobra.Command{
	Use:   "initialize",
	Short: "Initializes the AllSecure database and default properties",
	Long: `This command initializes the default properties and database of AllSecure
Here is the default usage, this command does not currently have an option for a config file. For example:

Setup.exe initialize -d "C:\Users\Allevon412\AllSecure\Database -l "C:\Users\Allevon412\AllSecure -u "Allevon" -p "AllSecure123"
The default settings for each command will be the current working directory for -l the -d will be current working directory + \Database
The username will be AllSecure the password will be AllSecure123`,

	Run: func(cmd *cobra.Command, args []string) {
		config.DatabaseDir = config.ProjectDir + "\\Database"
		_, err := config.CheckForAndCreateDatabase()
		if err != nil {
			log.Fatalln(err)
		}
		config.InitDatabase()
		fmt.Println("[+] Database Initialized and initial AllSecure user created!")
		config.WriteDefaultConfig()
		fmt.Println("[+] Server File Created with default values!")
	},
}

func init() {

	initializeCmd.Flags().StringVarP(&config.ProjectDir, "ProjectDir", "d", "", "C:\\Users\\Allevon412\\AllSecure")
	initializeCmd.Flags().StringVarP(&config.Username, "Username", "u", "AllSecure", "<username>")
	initializeCmd.Flags().StringVarP(&config.Password, "Password", "p", "AllSecure123", "<password>")

	rootCmd.AddCommand(initializeCmd)

	// Here you will define your flags and configuration settings.

	// Cobra supports Persistent Flags which will work for this command
	// and all subcommands, e.g.:
	// initializeCmd.PersistentFlags().String("foo", "", "A help for foo")

	// Cobra supports local flags which will only run when this command
	// is called directly, e.g.:
	// initializeCmd.Flags().BoolP("toggle", "t", false, "Help message for toggle")
}
