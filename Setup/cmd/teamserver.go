/*
Copyright © 2024 NAME HERE <EMAIL ADDRESS>
*/
package cmd

import (
	"log"
	"os"
	"os/exec"
	"strings"

	"github.com/spf13/cobra"
)

// teamserverCmd represents the teamserver command
var teamserverCmd = &cobra.Command{
	Use:   "teamserver",
	Short: "Starts the teamserver",
	Long:  `This command will start the teamserver for AllSecure, this must be started first for the client to have connectivity`,
	Run: func(cmd *cobra.Command, args []string) {
		path, err := os.Getwd()
		if err != nil {
			log.Fatalln("[error] attempting to get current working directory", err)
		}
		parts := strings.Split(path, "AllSecure")
		cmd2 := exec.Command(parts[0] + "\\AllSecure\\TeamServer\\TeamServer.exe")
		err = cmd2.Run()
		if err != nil {
			log.Fatalln("[error] attempting to run the teamserver", err)
		}
	},
}

func init() {
	rootCmd.AddCommand(teamserverCmd)

	// Here you will define your flags and configuration settings.

	// Cobra supports Persistent Flags which will work for this command
	// and all subcommands, e.g.:
	// teamserverCmd.PersistentFlags().String("foo", "", "A help for foo")

	// Cobra supports local flags which will only run when this command
	// is called directly, e.g.:
	// teamserverCmd.Flags().BoolP("toggle", "t", false, "Help message for toggle")
}
