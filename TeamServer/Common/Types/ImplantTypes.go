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
