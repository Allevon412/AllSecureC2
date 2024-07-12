#include "../../headers/helpers/ListManager.h"
#include "../../headers/cstdreplacement/localcstd.h"


pHostData AddHost(LPWSTR host, SIZE_T size, INT port) {
	pHostData data = NULL;
	
	//generate memory to store our agent data in the heap.
	data = agent->apis->pRtlAllocateHeap(NtProcessHeap(agent), HEAP_ZERO_MEMORY, sizeof(HostData));
	data->Host = agent->apis->pRtlAllocateHeap(NtProcessHeap(agent), HEAP_ZERO_MEMORY, size + sizeof(WCHAR));

	//store port
	data->Port = port;
	//host is alive
	data->Alive = TRUE;
	//set the host we just added to front of linked list.
	data->Next = agent->config->listenerConfig->Hosts;
	MemoryCopy(data->Host, host, size); // copy our host string.
	//reset our config's linkedlist to our new host at the front of line.
	agent->config->listenerConfig->Hosts = data;
	return data;
}

pHostData SelectHost(DWORD HostRotation) {
	pHostData data = agent->config->listenerConfig->Hosts;
	DWORD i = 0;
	//if we have no hosts return null.
	if (agent->config->listenerConfig->NumHosts == 0) {
		return NULL;
	}
	//if we have only one host return that host.
	if (agent->config->listenerConfig->NumHosts == 1) {
		return data;
	}
	switch (HostRotation) {
	case HOST_ROTATION_RANDOM:
		i = GenerateRandomNumber() % agent->config->listenerConfig->NumHosts;
		while (data != NULL && i > 0) {
			data = data->Next;
			i--;
		}
		agent->config->listenerConfig->CurrentHost = data;
		break;
	case HOST_ROTATION_ROUND_ROBIN:
		if (agent->config->listenerConfig->Hosts->Alive) {
			agent->config->listenerConfig->CurrentHost = agent->config->listenerConfig->Hosts;
		}
		else {
			data = agent->config->listenerConfig->Hosts;
			while (data != NULL && !data->Alive) {
				data = data->Next;
			}
			agent->config->listenerConfig->CurrentHost = data;
		}
		break;
	case HOST_ROTATION_FAIL_OVER:
		agent->config->listenerConfig->CurrentHost->Alive = FALSE;
		
		if (agent->config->listenerConfig->NumHosts > 1) { // ensure the number of hosts is greater than 1. othwerwise just keep trying the same host.
			while (data != NULL)
			{
				if (agent->config->listenerConfig->CurrentHost != data) { // the host that has failed is the current host.
					agent->config->listenerConfig->CurrentHost = data;
					break;
				}
				data = data->Next;
			}
		}
		
	}
	return data;
}

