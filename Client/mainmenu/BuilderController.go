package mainmenu

import (
	"Client/Common"
	"crypto/tls"
	"encoding/json"
	"errors"
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/dialog"
	"fyne.io/fyne/v2/widget"
	"log"
	"net/http"
	"strconv"
	"strings"
	"time"
)

func CreateImplantFunc(ImplantName, WorkingHours *Common.CustomEntry, KillDate time.Time, CustomHeaders string,
	Sleep, Jitter int, Arch string, Listener *widget.CheckGroup, MemoryEncryptionType, JmpType, Format, HostRotation string, clientobj *Common.Client, NewWindow fyne.Window) {
	var (
		ImplantData Common.ImplantConfig
		err         error
		Jdata       []byte
		transport   http.Transport
		client      http.Client
	)
	//close the form window at end of the function.
	defer NewWindow.Close()

	ImplantData.ListenerConfig.Method = "POST"
	ImplantData.ListenerConfig.UserAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.3"
	ImplantData.ListenerConfig.Headers = "Content-Type: */*"

	for _, val := range Listener.Selected {
		for _, val2 := range Common.ListenerTableEntries {
			ImplantData.ListenerConfig.Hosts = append(ImplantData.ListenerConfig.Hosts, val2.HOST+":"+strconv.Itoa(val2.PortBind))
			if val == val2.ListenerName {
				if val2.Protocol == "HTTPS" {
					ImplantData.ListenerConfig.ListenerType = 1
					ImplantData.ListenerConfig.Secure = true
				} else {
					ImplantData.ListenerConfig.ListenerType = 0
					ImplantData.ListenerConfig.Secure = false
				}
			}
		}
	}

	ImplantData.Name = ImplantName.Text
	ImplantData.ListenerConfig.Name = Listener.Selected[0]
	ImplantData.WorkingHours = WorkingHours.Text
	ImplantData.KillDate = KillDate.Unix()
	ImplantData.ListenerConfig.Headers = CustomHeaders
	ImplantData.Sleep = Sleep
	ImplantData.Jitter = Jitter
	ImplantData.ListenerConfig.HostHeader = Listener.Selected[0]
	if Arch == "x64" {
		ImplantData.Arch = 1
	} else {
		ImplantData.Arch = 2
	}

	if JmpType == "JMP RAX" {
		ImplantData.JmpType = false
	} else {
		ImplantData.JmpType = true
	}

	ImplantData.MemoryEncryptionType = MemoryEncryptionType
	ImplantData.Format = Format
	ImplantData.HostRotation = HostRotation

	//setup the http tls configuration
	transport.TLSClientConfig = &tls.Config{InsecureSkipVerify: true}
	client.Transport = &transport
	//setup our endpoint
	endpoint := "https://" + clientobj.Server + "/CreateImplant"

	//TODO remove debug output.
	log.Println("[info] sending implant data to server: ", ImplantData)
	log.Println("[info] kill date: ", KillDate.String())
	//parse our user bool input to bind it to our NewUser struct.
	//marshal the data for http request.
	Jdata, err = json.Marshal(ImplantData)
	if err != nil {
		log.Println("[error] attempting to marshal implant data", err)
		return
	}

	_, err = Common.PerformHTTPReq(clientobj, endpoint, Jdata)
	if err.Error() == "unauthorized" {
	} else if err != nil {
		log.Println("[error] attempting to create new implant", err)
	}

	return
}

func ImplantBuilderOnSubmitFunc(ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter *Common.CustomEntry,
	ListenerList *widget.CheckGroup, JmpType, MemoryEncryptionType, Format, HostRotation, Arch *widget.Select, clientobj *Common.Client, NewWindow fyne.Window) {
	if ImplantName.Text == "" || WorkingHours.Text == "" || KillDate.Text == "" || Sleep.Text == "" ||
		Jitter.Text == "" || JmpType.Selected == "" || MemoryEncryptionType.Selected == "" ||
		Format.Selected == "" {
		dialog.ShowInformation("Error", "Please fill out all fields", NewWindow)
		return
	} else {
		var count int
		for _, val := range ListenerList.Selected {
			if val == "" {
				count += 1
			}
		}
		if count == len(ListenerList.Selected) {
			dialog.ShowInformation("Error", "Please select a listener", NewWindow)
			return
		}

		if SleepVal, err := strconv.Atoi(Sleep.Text); SleepVal < 0 || err != nil {
			if err != nil {
				dialog.ShowInformation("Error", "likely the incorrect input information was given"+err.Error(), NewWindow)
				return
			}
			dialog.ShowInformation("Error", "Sleep must be greater than or equal to 0", NewWindow)
			return
		} else if JitterVal, err := strconv.Atoi(Jitter.Text); JitterVal < 0 || err != nil {
			if err != nil {
				dialog.ShowInformation("Error", "likely the incorrect input information was given"+err.Error(), NewWindow)
				return
			}
			dialog.ShowInformation("Error", "Jitter must be greater than 0", NewWindow)
			return
		} else if KillDateVal, err := time.Parse("2006-01-02 15:04:05 ", KillDate.Text); err != nil {
			dialog.ShowInformation("Error", "Invalid kill date format", NewWindow)
			return
		} else if vals := strings.Split(WorkingHours.Text, "-"); len(vals) != 2 {
			dialog.ShowInformation("Error", "Invalid working hours format", NewWindow)
			return
		} else if _, err := time.Parse("15:04", vals[0]); err != nil {
			dialog.ShowInformation("Error", "Invalid working hours format", NewWindow)
			return
		} else if _, err := time.Parse("15:04", vals[1]); err != nil {
			dialog.ShowInformation("Error", "Invalid working hours format", NewWindow)
			return
		} else {
			/*
				ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter *widget.Entry
					ListenerList                                                            *widget.Select
			*/
			CreateImplantFunc(ImplantName, WorkingHours, KillDateVal, CustomHeaders.Text, SleepVal, JitterVal, Arch.Selected,
				ListenerList, MemoryEncryptionType.Selected, JmpType.Selected, Format.Selected, HostRotation.Selected, clientobj, NewWindow)
		}
	}
}

func ImplantBuilderForm(clientobj *Common.Client, OldWindow fyne.App) error {

	NewWindow := OldWindow.NewWindow("Implant Builder Form")

	//TODO make the architecture a select option
	var (
		ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter *Common.CustomEntry
		JmpType, MemoryEncryptionType, Format, HostRotation, Arch         *widget.Select
		ListenerList                                                      *widget.CheckGroup
	)

	ImplantName = Common.NewCustomCredentialEntry(func() {
		ImplantBuilderOnSubmitFunc(ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter, ListenerList, JmpType, MemoryEncryptionType, Format, HostRotation, Arch, clientobj, NewWindow)
	})
	ImplantName.PlaceHolder = "Implant1"
	WorkingHours = Common.NewCustomCredentialEntry(func() {
		ImplantBuilderOnSubmitFunc(ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter, ListenerList, JmpType, MemoryEncryptionType, Format, HostRotation, Arch, clientobj, NewWindow)
	})
	WorkingHours.PlaceHolder = "8:00-17:00"
	CustomHeaders = Common.NewCustomCredentialEntry(func() {
		ImplantBuilderOnSubmitFunc(ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter, ListenerList, JmpType, MemoryEncryptionType, Format, HostRotation, Arch, clientobj, NewWindow)
	})
	CustomHeaders.PlaceHolder = "X-Forwarded-For: https://google.com, Referrer: https://bing.com"
	KillDate = Common.NewCustomCredentialEntry(func() {
		ImplantBuilderOnSubmitFunc(ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter, ListenerList, JmpType, MemoryEncryptionType, Format, HostRotation, Arch, clientobj, NewWindow)
	})
	KillDate.PlaceHolder = "2022-01-01 12:00:00"
	Sleep = Common.NewCustomCredentialEntry(func() {
		ImplantBuilderOnSubmitFunc(ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter, ListenerList, JmpType, MemoryEncryptionType, Format, HostRotation, Arch, clientobj, NewWindow)
	})
	Sleep.PlaceHolder = "1000"
	Jitter = Common.NewCustomCredentialEntry(func() {
		ImplantBuilderOnSubmitFunc(ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter, ListenerList, JmpType, MemoryEncryptionType, Format, HostRotation, Arch, clientobj, NewWindow)
	})
	Jitter.PlaceHolder = "100"

	ListenerList = widget.NewCheckGroup([]string{}, nil)
	JmpType = widget.NewSelect([]string{"JMP RAX", "JMP RBX"}, nil)
	MemoryEncryptionType = widget.NewSelect([]string{"None", "AES", "RC4", "ChaCha20"}, nil)
	Format = widget.NewSelect([]string{"PE", "DLL", "Shellcode"}, nil)
	HostRotation = widget.NewSelect([]string{"Random", "Round-Robin"}, nil)
	Arch = widget.NewSelect([]string{"x64", "x86"}, nil)

	for i, list := range Common.ListenerTableEntries {
		if i == 0 && list.ListenerName == "" {
			dialog.ShowInformation("Error", "Please fill out all fields", NewWindow)
			return errors.New("[error] attempting to create implant builder form. listener table is empty")
		}
		ListenerList.Options = append(ListenerList.Options, list.ListenerName)
	}

	//create a new form.
	form := &widget.Form{
		Items: []*widget.FormItem{
			{
				Text:     "Implant Name: ",
				Widget:   ImplantName,
				HintText: "SecureOne",
			},
			{
				Text:     "Working Hours: ",
				Widget:   WorkingHours,
				HintText: "8:00-17:00",
			},
			{
				Text:     "Kill Date: ",
				Widget:   KillDate,
				HintText: "2022-01-01 12:00:00",
			},
			{
				Text:     "Custom HTTP Headers (optional): ",
				Widget:   CustomHeaders,
				HintText: "Comma separated list of headers",
			},
			{
				Text:     "Sleep in Seconds: ",
				Widget:   Sleep,
				HintText: "1000",
			},
			{
				Text:     "Jitter in Seconds: ",
				Widget:   Jitter,
				HintText: "100",
			},
			{
				Text:   "Host Rotation",
				Widget: HostRotation,
			},
			{
				Text:   "Listeners: ",
				Widget: ListenerList,
			},
			//TODO add more options that are accurate.
			//The injection type / encryption type are just sample options to choose from.
			{
				Text:   "JMP Type: ",
				Widget: JmpType,
			},
			{
				Text:   "Memory Encryption Type: ",
				Widget: MemoryEncryptionType,
			},
			{
				Text:   "Format: ",
				Widget: Format,
			},
			{
				Text:   "Architecture: ",
				Widget: Arch,
			},
		},
		OnSubmit: func() {
			ImplantBuilderOnSubmitFunc(ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter,
				ListenerList, JmpType, MemoryEncryptionType, Format, HostRotation, Arch, clientobj, NewWindow)
		},
		OnCancel:   NewWindow.Close,
		SubmitText: "Create Implant",
		CancelText: "Exit",
	}

	//show our form in the new window.
	stack := container.NewStack(form)
	NewWindow.SetContent(stack)
	NewWindow.Resize(fyne.NewSize(700, 225))
	NewWindow.Show()
	return nil
}
