cmake_minimum_required(VERSION 3.28)
project(Agent_CLion)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")

set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -g")

set(ASSEMBLER_PATH "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Tools/MSVC/14.37.32822/bin/Hostx64/x64/ml64.exe")

set(ASM_SOURCE "${CMAKE_SOURCE_DIR}/src/agent/evasion/stackspoof/DesyncSpoofer.x64.asm")

set(ASM_OBJECT "${CMAKE_BINARY_DIR}/DesyncSpoofer.obj")

add_custom_command(
        OUTPUT ${ASM_OBJECT}
        COMMAND ${ASSEMBLER_PATH} /c /Fo ${ASM_OBJECT} ${ASM_SOURCE}
        DEPENDS ${ASM_SOURCE}
        COMMENT "Assembling ${ASM_SOURCE} with MASM"
)

add_executable(agent
        ${ASM_OBJECT}
        main.c
        src/agent/agent.c
        headers/agent/agent.h
        src/agent/init.c
        src/crypt/wolfssl/random.c
        headers/Crypt/wolfssl/wolfcrypt/wc_encrypt.h
        headers/Crypt/wolfssl/wolfcrypt/sha.h
        headers/agent/common/Common.h
        headers/agent/evasion/SleepObfu/EkkoSleepObf.h
        headers/agent/evasion/PeParsing/evasive.h
        headers/agent/evasion/SleepObfu/SleepObfMain.h
        headers/agent/agent.h
        headers/Crypt/wolfssl/wolfcrypt/aes.h
        headers/Crypt/wolfssl/wolfcrypt/asn.h
        headers/Crypt/wolfssl/wolfcrypt/asn_public.h
        headers/Crypt/wolfssl/wolfcrypt/coding.h
        headers/Crypt/wolfssl/wolfcrypt/cpuid.h
        headers/Crypt/wolfssl/wolfcrypt/cryptocb.h
        headers/Crypt/wolfssl/wolfcrypt/des3.h
        headers/Crypt/wolfssl/wolfcrypt/dh.h
        headers/Crypt/wolfssl/wolfcrypt/dsa.h
        headers/Crypt/wolfssl/wolfcrypt/error-crypt.h
        headers/Crypt/wolfssl/wolfcrypt/hash.h
        headers/Crypt/wolfssl/wolfcrypt/hmac.h
        headers/Crypt/wolfssl/wolfcrypt/hpke.h
        headers/Crypt/wolfssl/wolfcrypt/logging.h
        headers/Crypt/wolfssl/wolfcrypt/md2.h
        headers/Crypt/wolfssl/wolfcrypt/md5.h
        headers/Crypt/wolfssl/wolfcrypt/memory.h
        headers/Crypt/wolfssl/wolfcrypt/misc.h
        headers/Crypt/wolfssl/wolfcrypt/pkcs12.h
        headers/Crypt/wolfssl/wolfcrypt/pwdbased.h
        headers/Crypt/wolfssl/wolfcrypt/random.h
        headers/Crypt/wolfssl/wolfcrypt/rc2.h
        headers/Crypt/wolfssl/wolfcrypt/rsa.h
        headers/Crypt/wolfssl/wolfcrypt/settings.h
        headers/Crypt/wolfssl/wolfcrypt/sha.h
        headers/Crypt/wolfssl/wolfcrypt/sha256.h
        headers/Crypt/wolfssl/wolfcrypt/sp_int.h
        headers/Crypt/wolfssl/wolfcrypt/types.h
        headers/Crypt/wolfssl/wolfcrypt/visibility.h
        headers/Crypt/wolfssl/wolfcrypt/wc_encrypt.h
        headers/Crypt/wolfssl/wolfcrypt/wc_port.h
        headers/Crypt/wolfssl/wolfcrypt/wolfmath.h
        headers/Crypt/Crypto.h
        headers/helpers/ListManager.h
        headers/helpers/parser.h
        headers/package/package.h
        headers/taskcontroller/TaskController.h
        src/agent/RegisterAgent.c
        src/crypt/wolfssl/aes.c
        src/crypt/wolfssl/asn.c
        src/crypt/wolfssl/coding.c
        src/crypt/wolfssl/cryptocb.c
        src/crypt/wolfssl/des3.c
        src/crypt/wolfssl/dh.c
        src/crypt/wolfssl/dsa.c
        src/crypt/wolfssl/error.c
        src/crypt/wolfssl/hash.c
        src/crypt/wolfssl/hmac.c
        src/crypt/wolfssl/logging.c
        src/crypt/wolfssl/md2.c
        src/crypt/wolfssl/md5.c
        src/crypt/wolfssl/memory.c
        src/crypt/wolfssl/misc.c
        src/crypt/wolfssl/pkcs12.c
        src/crypt/wolfssl/pwdbased.c
        src/crypt/wolfssl/rc2.c
        src/crypt/wolfssl/rsa.c
        src/crypt/wolfssl/sha.c
        src/crypt/wolfssl/sha256.c
        src/crypt/wolfssl/sp_int.c
        src/crypt/wolfssl/wc_encrypt.c
        src/crypt/wolfssl/wc_port.c
        src/crypt/wolfssl/wolfmath.c
        src/crypt/Crypto.c
        headers/Crypt/wolfssl/opensssl/compat_types.h
        headers/agent/common/Common.h
        headers/agent/evasion/StackSpoof/Functions.h
        headers/agent/evasion/StackSpoof/SilentMoonwalk.h
        headers/agent/evasion/StackSpoof/SilentMoonwalkCommon.h
        headers/agent/evasion/StackSpoof/Spoof.h
        headers/agent/evasion/SleepObfu/EkkoSleepObf.h
        headers/agent/evasion/PeParsing/evasive.h
        headers/agent/evasion/SleepObfu/SleepObfMain.h
        headers/agent/agent.h
        headers/Crypt/wolfssl/opensssl/compat_types.h
        headers/Crypt/wolfssl/wolfcrypt/aes.h
        headers/Crypt/wolfssl/wolfcrypt/asn.h
        headers/Crypt/wolfssl/wolfcrypt/asn_public.h
        headers/Crypt/wolfssl/wolfcrypt/coding.h
        headers/Crypt/wolfssl/wolfcrypt/cpuid.h
        headers/Crypt/wolfssl/wolfcrypt/cryptocb.h
        headers/Crypt/wolfssl/wolfcrypt/des3.h
        headers/Crypt/wolfssl/wolfcrypt/dh.h
        headers/Crypt/wolfssl/wolfcrypt/dsa.h
        headers/Crypt/wolfssl/wolfcrypt/error-crypt.h
        headers/Crypt/wolfssl/wolfcrypt/hash.h
        headers/Crypt/wolfssl/wolfcrypt/hmac.h
        headers/Crypt/wolfssl/wolfcrypt/hpke.h
        headers/Crypt/wolfssl/wolfcrypt/logging.h
        headers/Crypt/wolfssl/wolfcrypt/md2.h
        headers/Crypt/wolfssl/wolfcrypt/md5.h
        headers/Crypt/wolfssl/wolfcrypt/memory.h
        headers/Crypt/wolfssl/wolfcrypt/misc.h
        headers/Crypt/wolfssl/wolfcrypt/pkcs12.h
        headers/Crypt/wolfssl/wolfcrypt/pwdbased.h
        headers/Crypt/wolfssl/wolfcrypt/random.h
        headers/Crypt/wolfssl/wolfcrypt/rc2.h
        headers/Crypt/wolfssl/wolfcrypt/rsa.h
        headers/Crypt/wolfssl/wolfcrypt/settings.h
        headers/Crypt/wolfssl/wolfcrypt/sha.h
        headers/Crypt/wolfssl/wolfcrypt/sha256.h
        headers/Crypt/wolfssl/wolfcrypt/sp_int.h
        headers/Crypt/wolfssl/wolfcrypt/types.h
        headers/Crypt/wolfssl/wolfcrypt/visibility.h
        headers/Crypt/wolfssl/wolfcrypt/wc_encrypt.h
        headers/Crypt/wolfssl/wolfcrypt/wc_port.h
        headers/Crypt/wolfssl/wolfcrypt/wolfmath.h
        headers/Crypt/wolfssl/wolfssl/callbacks.h
        headers/Crypt/wolfssl/wolfssl/version.h
        headers/Crypt/Crypto.h
        headers/cstdreplacement/localcstd.h
        headers/enumeration/enum.h
        headers/enumeration/token.h
        headers/helpers/ListManager.h
        headers/helpers/parser.h
        headers/http/Http.h
        headers/package/package.h
        headers/taskcontroller/TaskController.h
        src/agent/evasion/stackspoof/SilentMoonwalk.c
        src/agent/evasion/SleepObfu/EkkoSleepObf.c
        src/agent/evasion/PeParsing/evasive.c
        src/agent/evasion/SleepObfu/SleepObfMain.c
        src/cstdreplacement/localcstd.c
        src/enumeration/enum.c
        src/enumeration/token.c
        src/helpers/ListManager.c
        src/helpers/parser.c
        src/http/Http.c
        src/package/package.c
        src/TaskController/TaskController.c
        src/agent/evasion/syscalls/syscalls.c
        headers/agent/evasion/syscalls/syscalls.h
        headers/agent/evasion/Common.h
        src/agent/evasion/common.c
)
