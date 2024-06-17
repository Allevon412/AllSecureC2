#include "package.h"
#include "localcstd.h"

/*
	This section is almost a 1 for 1 of the code from havoc package. I'll try to make edits where I can. But I thought  the solution was very elegant so i wanted to adopt it.
*/

pPackage CreatePackage(UINT32 CommandID) {
	pPackage pack = NULL;

	//allocate the pointer to memory location for the total package.
	pack = agent->apis->pLocalAlloc(LPTR, sizeof(Package));
	//allocate the pointer to the memory location for the package's data buffer.
	pack->Buffer = agent->apis->pLocalAlloc(LPTR, sizeof(BYTE));
	//set data buffer length to 0.
	pack->Length = 0;
	//set request identifier to 1 (first request).
	pack->RequestID = 1;
	//set command id to the command used to create the package.
	pack->CommandID = CommandID;
	//encrypt the data.
	pack->Encrypt = TRUE;
	//destroy the package after the package has been sent.
	pack->Destroy = TRUE;
	//next package node incase there's a queue waiting to be sent.
	pack->Next = NULL;

	return pack;
}

pPackage CreatePackageWithMetaData(UINT32 CommandID) {
	pPackage pack = CreatePackage(CommandID);
	int err = 0;

	if ((err = AddInt32ToPackage(pack, 0)) != PACKAGE_SUCCESS) { //Setting the first four bytes of buffer to 0. This will allow us to put the length field in later @ beginning of buffer.
		//could also probably do this using the last four bytes of the buffer as well.
		printf("[error] attempting to add four bytes to package. %s\n", PackageErrorToString(err));
		return NULL;
	}
	if ((err = AddInt32ToPackage(pack, AGENT_MAGIC_VALUE)) != PACKAGE_SUCCESS) {
		printf("[error] attempting to add four bytes to package. %s\n", PackageErrorToString(err));
		return NULL;
	}
	if ((err = AddInt32ToPackage(pack, agent->config->AgentID)) != PACKAGE_SUCCESS) {
		printf("[error] attempting to add four bytes to package. %s\n", PackageErrorToString(err));
		return NULL;
	}
	if ((err = AddInt32ToPackage(pack, pack->CommandID)) != PACKAGE_SUCCESS) {
		printf("[error] attempting to add four bytes to package. %s\n", PackageErrorToString(err));
		return NULL;
	}
	if ((err = AddInt32ToPackage(pack, pack->RequestID)) != PACKAGE_SUCCESS) {
		printf("[error] attempting to add four bytes to package. %s\n", PackageErrorToString(err));
		return NULL;
	}
	if ((err = AddInt32ToPackage(pack, StringLengthA(agent->config->AgentName))) != PACKAGE_SUCCESS) {
		printf("[error] attempting to add agent name length to package. %s\n", PackageErrorToString(err));
		return NULL;
	}
	if ((err = AddStringToPackage(pack, agent->config->AgentName)) != PACKAGE_SUCCESS) {
		printf("[error] attempting to add string to package. %s\n", PackageErrorToString(err));
		return NULL;
	}

	return pack;
}

void AddInt32ToBuffer(PUCHAR buffer, UINT32 data) {
	//Data example: 4563EFCD
	//Data >> 24 & 0xFF = 45
	buffer[0] = (data >> 24) & 0xFF;
	//Data >> 16 & 0xFF = 63
	buffer[1] = (data >> 16) & 0xFF;
	//Data >> 8 & 0xFF = EF
	buffer[2] = (data >> 8) & 0xFF;
	//Data & 0xFF = CD
	buffer[3] = (data) & 0xFF;
}

INT AddInt32ToPackage(pPackage pack, UINT32 data) {

	//check if we have valid package.
	if (!pack) 
		return PACKAGE_DOES_NOT_EXIST;

	//reallocate memory buffer with extended length.
	pack->Buffer = agent->apis->pLocalReAlloc(pack->Buffer,
		pack->Length + sizeof(UINT32), 
		LMEM_MOVEABLE
	);

	if (!pack->Buffer)
		return PACKAGE_BUFFER_REALLOC_FAILED;

	//add our data in the extended buffer slot.
	AddInt32ToBuffer((PUCHAR)pack->Buffer + pack->Length, data);
	//add additional length to header.
	pack->Length += sizeof(UINT32);

	//return success.
	return PACKAGE_SUCCESS;
}

//edited this from the solution in havoc. Just used the solution from the add 32 to buff function.
void AddInt64ToBuffer(PUCHAR buffer, UINT64 data) {
	
	//data example:
	//0xAABBCCDDEEFF8877
	//buffer[7] = 77
	buffer[7] = data & 0xff;
	//buffer[6] = 88
	buffer[6] = data >> 8 & 0xff;
	//buffer[5] = FF
	buffer[5] = data >> 16 & 0xff;
	//buffer[4] = EE
	buffer[4] = data >> 24 & 0xff;
	//buffer[3] = DD
	buffer[3] = data >> 32 & 0xff;
	//buffer[2] = CC
	buffer[2] = data >> 40 & 0xff; 
	//buffer[1] = BB
	buffer[1] = data >> 48 & 0xff;
	//buffer[0] = AA
	buffer[0] = data >> 56 & 0xff;

}

INT AddInt64ToPackage(pPackage pack, UINT64 data) {
	//check if we have a valid package.
	if (!pack) {
		return PACKAGE_DOES_NOT_EXIST;
	}
	//reallocate memory buffer with extended length
	pack->Buffer = agent->apis->pLocalReAlloc(pack->Buffer, 
		pack->Length + sizeof(UINT64), 
		LMEM_MOVEABLE
	);
	if (!pack->Buffer)
		return PACKAGE_BUFFER_REALLOC_FAILED;

	//add our data in the extended buffer slot.
	AddInt64ToBuffer((PUCHAR)pack->Buffer + pack->Length, data);
	//add additional length to header/
	pack->Length += sizeof(UINT64);

	//return success
	return PACKAGE_SUCCESS;
}

INT AddBoolToPackage(pPackage pack, BOOLEAN data) {

	//check if package exists
	if (!pack) {
		return PACKAGE_DOES_NOT_EXIST;
	}

	//reallocate memory buffer with extended length.
	pack->Buffer = agent->apis->pLocalReAlloc(pack->Buffer,
		pack->Length + sizeof(UINT32),  // using uint32 for size rather than single byte for bool. Keeps the size uniform & reduces paddding needs.
		LMEM_MOVEABLE
	);
	if (!pack->Buffer)
		return PACKAGE_BUFFER_REALLOC_FAILED;

	AddInt32ToBuffer((PUCHAR)pack->Buffer + pack->Length, data ? 1 : 0); //if data is true then send a true value (1) if data is false send a false value (0);

	pack->Length += sizeof(UINT32);

	return PACKAGE_SUCCESS;
}


//modified code to use 64bit pointers or 32bit pointers depending on what architecture the implant was compiled for.
INT AddPtrToPackage(pPackage pack, PVOID pointer) {
	
	int err = 0;
	//check if package exists
	if (!pack) {
		return PACKAGE_DOES_NOT_EXIST;
	}

	if (!pointer)
		return PACKAGE_POINTER_DOES_NOT_EXIST;

	//check pointer size.
	if (sizeof(PVOID) == sizeof(UINT32)) {

		if ((err = AddInt32ToPackage(pack, (UINT32)pointer)) != PACKAGE_SUCCESS) {
			printf("error attempting to add 32bit pointer to package\n");
			return err;
		}
	}
	else {
		if ((err = AddInt64ToPackage(pack, (UINT64)pointer)) != PACKAGE_SUCCESS) {
			printf("error attempting to add 64bit pointer to package\n");
			return err;
		}
	}

	return err;
}

INT AddPaddingToPackage(pPackage pack, PCHAR data, SIZE_T size) {

	if (!pack)
		return PACKAGE_DOES_NOT_EXIST;

	if (!data)
		return PACKAGE_DATA_DOES_NOT_EXIST;

	pack->Buffer = agent->apis->pLocalReAlloc(pack->Buffer,
		pack->Length + size,
		LMEM_MOVEABLE | LMEM_ZEROINIT
	);
	if (!pack->Buffer)
		return PACKAGE_BUFFER_REALLOC_FAILED;

	MemoryCopy((PUCHAR)pack->Buffer + pack->Length, data, size);

	pack->Length += size;

	return PACKAGE_SUCCESS;
}
//TODO REWRITE THIS FUNCTION TO ADD BYTE SIZE AS PREFIX TO BYTES PAYLOAD.
//THEN EDIT ALL STRING / BYTES ADD PACKAGE FUNCTION CALLS TO REMOVE THE ADD32INT BEFORE
INT AddBytesToPackage(pPackage pack, PBYTE data, SIZE_T size) {

	if (!pack)
		return PACKAGE_DOES_NOT_EXIST;

	if (!data)
		return PACKAGE_DATA_DOES_NOT_EXIST;

	if (!size || size < 1)
		return PACKAGE_UNEXPECTED_DATA_SIZE;

	pack->Buffer = agent->apis->pLocalReAlloc(pack->Buffer, 
		pack->Length + size, 
		LMEM_MOVEABLE | LMEM_ZEROINIT
	);
	
	if (!pack->Buffer)
		return PACKAGE_BUFFER_REALLOC_FAILED;

	MemoryCopy((PUCHAR)pack->Buffer + pack->Length, data, size);
	pack->Length += size; 

	return PACKAGE_SUCCESS;
}

INT AddStringToPackage(pPackage pack, PCHAR data) {
	return AddBytesToPackage(pack, (PBYTE)data, StringLengthA(data));
}

INT AddWStringToPackage(pPackage pack, PWCHAR data) {
	return AddBytesToPackage(pack, (PBYTE)data, StringLengthW(data) * 2);
}

INT DestroyPackage(pPackage pack) {
	pPackage pkg = agent->packages;

	if (!pack)
		return PACKAGE_DOES_NOT_EXIST;

	while (pkg)
	{
		if (pack == pkg) {
			//TODO write this. and destory if package has been sent and is still apart of the agent packages list. May need to make a Doubly linked list to keep the list intact if a pacakge that is destoryed is in the middle of the list.
			return PACKAGE_HAS_NOT_BEEN_SENT_TO_SERVER; //do not destroy if it's in our list of packages to send.
		}
		pkg = pkg->Next;
	}

	if (pack->Buffer)
	{
		MemorySet(pack->Buffer, 0, pack->Length);
		agent->apis->pLocalFree(pack->Buffer);
		pack->Buffer = NULL;
	}


	MemorySet(pack, 0, sizeof(Package));
	agent->apis->pLocalFree(pack);
	pack = NULL;

	return PACKAGE_SUCCESS;
}

LPSTR PackageErrorToString(INT error) {
	LPSTR errorstr[100];
	switch (error)
	{
	case   PACKAGE_FALIURE_GENERIC:
		sprintf_s(errorstr,sizeof(errorstr), "GENERIC PACKAGE FAILURE\n");
		break;
	case PACKAGE_DOES_NOT_EXIST:
		sprintf_s(errorstr, sizeof(errorstr), "PACKAGE_DOES_NOT_EXIST\n");
		break;
	case PACKAGE_DATA_DOES_NOT_EXIST:
		sprintf_s(errorstr, sizeof(errorstr), "PACKAGE_DATA_DOES_NOT_EXIST\n");
		break;
	case PACKAGE_UNEXPECTED_DATA_SIZE:
		sprintf_s(errorstr, sizeof(errorstr), "PACKAGE_UNEXPECTED_DATA_SIZE\n");
		break;
	case PACKAGE_BUFFER_REALLOC_FAILED:
		sprintf_s(errorstr, sizeof(errorstr), "PACKAGE_BUFFER_REALLOC_FAILED\n");
		break;
	case PACKAGE_BUFFER_DOES_NOT_EXIST:
		sprintf_s(errorstr, sizeof(errorstr), "PACKAGE_BUFFER_DOES_NOT_EXIST\n");
		break;
	case PACKAGE_POINTER_DOES_NOT_EXIST:
		sprintf_s(errorstr, sizeof(errorstr), "PACKAGE_POINTER_DOES_NOT_EXIST\n");
		break;
	case PACKAGE_HAS_NOT_BEEN_SENT_TO_SERVER:
		sprintf_s(errorstr, sizeof(errorstr), "PACKAGE_HAS_NOT_BEEN_SENT_TO_SERVER\n");
		break;
	}
	return errorstr;
}

INT PackageSendMetaDataPackage(pPackage pack, PVOID pResponse, PSIZE_T pSize) {
	BOOL Success;
	if (!pack)
		return PACKAGE_DOES_NOT_EXIST;

	if (!pack->Buffer)
		return PACKAGE_BUFFER_DOES_NOT_EXIST;

	AddInt32ToBuffer(pack->Buffer, pack->Length); // add the length of the buffer to the front of buffer in empty space created when calling create metadata package.

	if (pack->Encrypt)
	{
		INT err;
		//TODO perform encryption.
		if ((err = AESCTR(
			(BYTE*)pack->Buffer + (PACKAGE_HEADER_LENGTH + sizeof(INT) + StringLengthA(agent->config->AgentName) + agent->EncryptedAESKeySize + agent->EncryptedIVSize),	//buffer to encrypt. BUFF START + HEADERS + AgentName Length + AgentName String Length (part of Headers) + ENCRYPTED KEY LENGTH + PREENCRYPTED AESKEY + ENCRYPTED IV LENGTH + PREENCRYPTED IV
			pack->Length - (PACKAGE_HEADER_LENGTH + sizeof(INT) + StringLengthA(agent->config->AgentName) + agent->EncryptedAESKeySize + agent->EncryptedIVSize),			//SIZE OF BUFFER TO ENCRYPT. BUFFER LEN - (HEADERS + AgentName Length + AgentName String Length (part of Headers)) - (ENCRYPTED KEY LENGTH + PREENCRYPTED AESKEY + ENCRYPTED IV LENGTH + PREENCRYPTED IV)
			agent->AESKey, AES_256_KEY_SIZE, agent->IV)																//AES KEY, AES KEY SIZE, IV
			) != 0) {
			return err;
		}
	}

	//TODO perform sending of package.
	if (!(Success = SendRegisterRequest(agent->MetaDataPackage->Buffer, agent->MetaDataPackage->Length))) {
		return PACKAGE_HAS_NOT_BEEN_SENT_TO_SERVER;
	}

	return PACKAGE_SUCCESS;
} 

BOOL PackageSendAll(OUT pDataBuffer Response, OUT PSIZE_T Size)
{
	BOOL Success = FALSE;
	pPackage pack = NULL;
	pPackage AgentPackList = agent->packages;
	pPackage PrevPackage = NULL;
	pPackage CurrentPackage = NULL;
	UINT32 Offset = 0;

	pack = CreatePackageWithMetaData(GET_AGENT_JOB);
	//coalesce all waiting packages into one big pacakge.
	while (AgentPackList)
	{
		AddInt32ToPackage(pack, AgentPackList->CommandID);
		AddInt32ToPackage(pack, AgentPackList->RequestID);
		AddBytesToPackage(pack, AgentPackList->Buffer, AgentPackList->Length);
		AgentPackList->Sent = TRUE;
		PrevPackage = AgentPackList;
		AgentPackList = AgentPackList->Next;
	}
	AddInt32ToBuffer(pack->Buffer, pack->Length - sizeof(UINT32));
	/*
 *  Header:
 *  [ SIZE         ] 4 bytes
 *  [ Magic Value  ] 4 bytes
 *  [ Agent ID     ] 4 bytes
 *  [ COMMAND ID   ] 4 bytes
 *  [ Request ID   ] 4 bytes
 *  [ Agent Name   ] size + bytes
*/
	Offset = (sizeof(UINT32) * 6) + StringLengthA(agent->config->AgentName);
	//encrypt the big package.
	AESCTR((BYTE*)pack->Buffer + Offset, pack->Length - Offset, agent->AESKey, agent->AESKeySize, agent->IV);
	//send the big package.
	if ((PerformRequest(pack->Buffer, pack->Length, Response, Size)) == 0)
	{
		Success = TRUE;
	}
	else {
		Success = FALSE;
	}
	//decrypt the payload.
	AESCTR((BYTE*)pack->Buffer + Offset, pack->Length - Offset, agent->AESKey, agent->AESKeySize, agent->IV);

	CurrentPackage = agent->packages;
	PrevPackage = NULL;

	if (Success)
	{
		//sending of all the packages worked. Now we can destroy them.
		while (CurrentPackage)
		{
			if (CurrentPackage->Sent)
			{
				//is this the first pacakge in our list.
				if (CurrentPackage == agent->packages)
				{
					//update our package list to the next pacakge.
					agent->packages = CurrentPackage->Next; // the reeason why our destroy function will work is because we move the pointer to the next package in the list. Thereefore removing the previous package from the list.
															// this will ensure we pass the check for "if this is still in list do not destroy in our package destroy function"
					//destroy if instructed to do so.
					if (CurrentPackage->Destroy)
					{
						DestroyPackage(CurrentPackage);
						CurrentPackage = NULL;
					}
					CurrentPackage = agent->packages;
					PrevPackage = NULL;
				}
				else
				{
					if (PrevPackage)
					{
						PrevPackage->Next = CurrentPackage->Next;
						if (CurrentPackage->Destroy)
						{
							DestroyPackage(CurrentPackage);
						}
						CurrentPackage = PrevPackage->Next;
					}
					else
					{
						printf("[error] attempting to clean up packages\n");
						continue;
					}
				}
			}
			else
			{
				PrevPackage = CurrentPackage;
				CurrentPackage = CurrentPackage->Next;
			}
		}
	}
	else //request to send all packages failed.
	{
		//make all packages as unsent.
		while (CurrentPackage)
		{
			CurrentPackage->Sent = FALSE;
			CurrentPackage = CurrentPackage->Next;
		}
	}

	DestroyPackage(pack);

	return Success;
}

void AddPackageToAgentPackageList(pPackage pack) {
	if (agent->packages == NULL)
	{
		agent->packages = pack;
		return;
	}
	else {
		pPackage temp;
		temp = agent->packages;
		while (temp->Next != NULL) {
			temp = temp->Next;
		}
		temp->Next = pack;
	}
}

