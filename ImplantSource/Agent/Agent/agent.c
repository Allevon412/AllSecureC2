#include "agent.h"

void AgentMain() {
	pAgent agent = (Agent*)malloc(sizeof(agent));
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