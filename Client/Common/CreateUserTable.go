package Common

type UserTableHeaders struct {
	HeaderNames []string
}
type UserData struct {
	UserID          int
	UserName        string
	Admin           bool
	ActiveImplants  string
	ActiveListeners string
}

var (
	UserHeaders UserTableHeaders
	UserRows    []UserData
)

func UserTableInit() {
	UserHeaders.HeaderNames = []string{"User ID", "Username", "Administrator", "Active Implants", "Active Listeners"}

}
