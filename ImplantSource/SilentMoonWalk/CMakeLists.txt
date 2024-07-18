cmake_minimum_required(VERSION 3.28)
project(SilentMoonWalk)

set(CMAKE_CXX_STANDARD 17)

set(ASSEMBLER_PATH "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Tools/MSVC/14.37.32822/bin/Hostx64/x64/ml64.exe")

set(ASM_SOURCE "${CMAKE_SOURCE_DIR}/src/DesyncSpoofer.asm")

set(ASM_SOURCE2 "${CMAKE_SOURCE_DIR}/src/SyntheticSpoofer.asm")

set(ASM_OBJECT "${CMAKE_BINARY_DIR}/DesyncSpoofer.obj")

set(ASM_OBJECT2 "${CMAKE_BINARY_DIR}/SyntheticSpoofer.obj")

add_custom_command(
        OUTPUT ${ASM_OBJECT}
        COMMAND ${ASSEMBLER_PATH} /c /Fo ${ASM_OBJECT} ${ASM_SOURCE}
        DEPENDS ${ASM_SOURCE}
        COMMENT "Assembling ${ASM_SOURCE} with MASM"

        OUTPUT ${ASM_OBJECT2}
        COMMAND ${ASSEMBLER_PATH} /c /Fo ${ASM_OBJECT2} ${ASM_SOURCE2}
        DEPENDS ${ASM_SOURCE2}
        COMMENT "Assembling ${ASM_SOURCE2} with MASM"
)

add_executable(SilentMoonWalk SilentMoonwalk.cpp
        ${ASM_OBJECT}
        ${ASM_OBJECT2}
        headers/AddressHunter.h
        headers/Common.h
        headers/Functions.h
        headers/Spoof.h
        headers/Test.h)

if(MSVC)
    target_compile_options(SilentMoonWalk PRIVATE /wd4996)
else()
    target_compile_options(SilentMoonWalk PRIVATE -fpermissive)
endif ()
