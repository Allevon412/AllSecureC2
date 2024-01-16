package Common

type (
	CookieStruct struct {
	}

	Client struct {
		Server   string
		Username string
		Password string
		Admin    bool
		Cookie   CookieStruct
	}
)
