package Common

import "net/http"

type (
	CookieStruct struct {
		Token jwttoken
	}

	Client struct {
		Server    string
		Username  string
		Password  string
		Admin     bool
		Cookie    CookieStruct
		ClientObj http.Client
	}
)
