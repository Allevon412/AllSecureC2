/*
Copyright © 2024 NAME HERE <EMAIL ADDRESS>
*/
package cmd

import (
	"github.com/spf13/cobra"
	"log"
	"os"
	"os/exec"
	"strings"
)

// clientCmd represents the client command
var clientCmd = &cobra.Command{
	Use:   "client",
	Short: "This command starts the AllSecure client application",
	Long: `This command will start the AllSecure client application
The client will use HTTPS endpoints to authenticate to the framework & perform tasks.
`,
	Run: func(cmd *cobra.Command, args []string) {
		path, err := os.Getwd()
		if err != nil {
			log.Fatalln("[error] attempting to get current working directory", err)
		}
		parts := strings.Split(path, "AllSecure")
		cmd2 := exec.Command(parts[0] + "\\AllSecure\\Client\\Client.exe")
		err = cmd2.Run()
		if err != nil {
			log.Fatalln("[error] attempting to run the client app", err)
		}
	},
}

func init() {
	rootCmd.AddCommand(clientCmd)

	// Here you will define your flags and configuration settings.

	// Cobra supports Persistent Flags which will work for this command
	// and all subcommands, e.g.:
	// clientCmd.PersistentFlags().String("foo", "", "A help for foo")

	// Cobra supports local flags which will only run when this command
	// is called directly, e.g.:
	// clientCmd.Flags().BoolP("toggle", "t", false, "Help message for toggle")
}
