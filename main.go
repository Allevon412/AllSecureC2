package main

import (
	"AllSecure/ListeningServer"
)

func main() {
	var Handler ListeningServer.HTTPServer
	Handler.Start()
}
