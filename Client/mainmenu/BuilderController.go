package mainmenu

import (
	"Client/Common"
	"errors"
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/dialog"
	"fyne.io/fyne/v2/widget"
	"strconv"
	"strings"
	"time"
)

func CreateImplantFunc(ImplantName, WorkingHours *Common.CustomEntry, KillDate time.Time, CustomHeaders string,
	Sleep, Jitter int, Arch string, Listener *widget.CheckGroup, MemoryEncryptionType, InjectionType string, clientobj *Common.Client) {
	/*
		var (
			ImplantData Common.ImplantTableData
			err         error
		)
		//close the form window at end of the function.
		defer NewWindow.Close()

		ImplantData.ImplantName = ImplantName.Text
		ImplantData.WorkingHours = WorkingHours.Text
		ImplantData.KillDate = KillDate
		ImplantData.CustomHeaders = CustomHeaders
		ImplantData.Sleep, err = strconv.Atoi(Sleep)
		if err != nil {
			dialog.ShowInformation("Error", "likely the incorrect input information was given"+err.Error(), NewWindow)
			return
		}
		ImplantData.Jitter, err = strconv.Atoi(Jitter)
		if err != nil {
			dialog.ShowInformation("Error", "likely the incorrect input information was given"+err.Error(), NewWindow)
			return
		}
		ImplantData.Listener = Listener
		ImplantData.MemoryEncryptionType = MemoryEncryptionType
		ImplantData.InjectionType = InjectionType

		//append implant to our implant table.
		Common.ImplantData = append(Common.ImplantData, ImplantData)
		Common.ImplantTable.Refresh()
		//exit. implant added!
		return
	*/
	return
}

func ImplantBuilderOnSubmitFunc(ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter, Arch *Common.CustomEntry,
	ListenerList *widget.CheckGroup, InjectionType, MemoryEncryptionType *widget.Select, clientobj *Common.Client, NewWindow fyne.Window) {
	if ImplantName.Text == "" || WorkingHours.Text == "" || KillDate.Text == "" || Sleep.Text == "" ||
		Jitter.Text == "" || Arch.Text == "" || InjectionType.Selected == "" || MemoryEncryptionType.Selected == "" {
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

		if Arch.Text != "x64" && Arch.Text != "x86" {
			dialog.ShowInformation("Error", "Architecture must be x64 or x86", NewWindow)
			return
		} else if SleepVal, err := strconv.Atoi(Sleep.Text); SleepVal < 0 || err != nil {
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
		} else if _, err := time.Parse("2006-01-02 15:04:05", KillDate.Text); err != nil {
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
			KillDateVal, err := time.Parse("2022-01-01 12:01:02", KillDate.Text)
			if err != nil {
				dialog.ShowInformation("Error", "Invalid kill date format", NewWindow)
				return
			}
			/*
				ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter *widget.Entry
					ListenerList                                                            *widget.Select
			*/
			CreateImplantFunc(ImplantName, WorkingHours, KillDateVal, CustomHeaders.Text, SleepVal, JitterVal, Arch.Text,
				ListenerList, MemoryEncryptionType.Selected, InjectionType.Selected, clientobj)
		}
	}
}

func ImplantBuilderForm(clientobj *Common.Client, OldWindow fyne.App) error {

	NewWindow := OldWindow.NewWindow("Implant Builder Form")

	var (
		ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter, Arch, HostRotation *Common.CustomEntry
		InjectionType, MemoryEncryptionType                                                   *widget.Select
		ListenerList                                                                          *widget.CheckGroup
	)

	ImplantName = Common.NewCustomCredentialEntry(func() {
		ImplantBuilderOnSubmitFunc(ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter, Arch, ListenerList, InjectionType, MemoryEncryptionType, clientobj, NewWindow)
	})
	ImplantName.PlaceHolder = "Implant1"
	WorkingHours = Common.NewCustomCredentialEntry(func() {
		ImplantBuilderOnSubmitFunc(ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter, Arch, ListenerList, InjectionType, MemoryEncryptionType, clientobj, NewWindow)
	})
	WorkingHours.PlaceHolder = "8:00-17:00"
	CustomHeaders = Common.NewCustomCredentialEntry(func() {
		ImplantBuilderOnSubmitFunc(ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter, Arch, ListenerList, InjectionType, MemoryEncryptionType, clientobj, NewWindow)
	})
	CustomHeaders.PlaceHolder = "X-Forwarded-For: https://google.com"
	KillDate = Common.NewCustomCredentialEntry(func() {
		ImplantBuilderOnSubmitFunc(ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter, Arch, ListenerList, InjectionType, MemoryEncryptionType, clientobj, NewWindow)
	})
	KillDate.PlaceHolder = "2022-01-01 12:00:00"
	Sleep = Common.NewCustomCredentialEntry(func() {
		ImplantBuilderOnSubmitFunc(ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter, Arch, ListenerList, InjectionType, MemoryEncryptionType, clientobj, NewWindow)
	})
	Sleep.PlaceHolder = "1000"
	Jitter = Common.NewCustomCredentialEntry(func() {
		ImplantBuilderOnSubmitFunc(ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter, Arch, ListenerList, InjectionType, MemoryEncryptionType, clientobj, NewWindow)
	})
	Jitter.PlaceHolder = "100"
	Arch = Common.NewCustomCredentialEntry(func() {
		ImplantBuilderOnSubmitFunc(ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter, Arch, ListenerList, InjectionType, MemoryEncryptionType, clientobj, NewWindow)
	})
	Arch.PlaceHolder = "x64"
	HostRotation = Common.NewCustomCredentialEntry(func() {
		ImplantBuilderOnSubmitFunc(ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter, Arch, ListenerList, InjectionType, MemoryEncryptionType, clientobj, NewWindow)
	})
	HostRotation.PlaceHolder = "random || round-robin"

	ListenerList = widget.NewCheckGroup([]string{}, nil)
	InjectionType = widget.NewSelect([]string{}, nil)
	MemoryEncryptionType = widget.NewSelect([]string{}, nil)

	for i, list := range Common.ListenerTableEntries {
		if i == 0 && list.ListenerName == "" {
			dialog.ShowInformation("Error", "Please fill out all fields", NewWindow)
			return errors.New("[error] attempting to create implant builder form. listener table is empty")
		}
		ListenerList.Options = append(ListenerList.Options, list.ListenerName)
	}
	InjectionType.SetOptions([]string{"Shellcode", "Reflective DLL", "Shellcode + Reflective DLL"})
	MemoryEncryptionType.SetOptions([]string{"None", "AES", "RC4", "ChaCha20"})

	//create a new form.
	form := &widget.Form{
		Items: []*widget.FormItem{
			{
				Text:     "Implant Name: ",
				Widget:   ImplantName,
				HintText: "SecureOne",
			},
			{
				Text:     "Architecture: ",
				Widget:   Arch,
				HintText: "x64 || x86",
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
				HintText: "X-Forwarded-For: https://google.com",
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
				Text:     "Host Rotation",
				Widget:   HostRotation,
				HintText: "round-robin",
			},
			{
				Text:   "Listener: ",
				Widget: ListenerList,
			},
			//TODO add more options that are accurate.
			//The injection type / encryption type are just sample options to choose from.
			{
				Text:   "Injection Type: ",
				Widget: InjectionType,
			},
			{
				Text:   "Memory Encryption Type: ",
				Widget: MemoryEncryptionType,
			},
		},
		OnSubmit: func() {
			ImplantBuilderOnSubmitFunc(ImplantName, WorkingHours, CustomHeaders, KillDate, Sleep, Jitter, Arch,
				ListenerList, InjectionType, MemoryEncryptionType, clientobj, NewWindow)
		}, // OnSubmit()
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
