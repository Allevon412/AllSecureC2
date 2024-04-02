#include "package.h"

/*
	This section is almost a 1 for 1 of the code from havoc package. I'll try to make edits where I can. But I thought  the solution was very elegant so i wanted to adopt it.
*/

pPackage CreatePackage(UINT32 CommandID) {
	pPackage pack = NULL;

	//allocate the pointer to memory location for the total package.
	pack = LocalAlloc(LPTR, sizeof(Package));
	//allocate the pointer to the memory location for the package's data buffer.
	pack->Buffer = localAlloc(LPTR, sizeof(BYTE));
	//set data buffer length to 0.
	pack->Length = 0;
	//set request identifier to 1 (first request).
	pack->RequestID = 1;
	//set command id to the command used to create the package.
	pack->CommandID;
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

	if ((err = AddFourBytesToPackage(pack, 0)) != PACKAGE_SUCCESS) {
		printf("[error] attempting to add four bytes to package.\n");
		return NULL;
	}
	AddInt32ToPackage(pack, AGENT_MAGIC_VALUE);
	AddInt32ToPackage(pack, g_ImplantData.AgentID);
	AddInt32ToPackage(pack, pack->CommandID);
	AddInt32ToPackage(pack, pack->RequestID);

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
	pack->Buffer = LocalReAlloc(pack->Buffer, 
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
	pack->Buffer = LocalReAlloc(pack->Buffer, 
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
	pack->Buffer = LocalReAlloc(pack->Buffer, 
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

	pack->Buffer = LocalReAlloc(pack->Buffer,
		pack->Length + size,
		LMEM_MOVEABLE | LMEM_ZEROINIT
	);
	if (!pack->Buffer)
		return PACKAGE_BUFFER_REALLOC_FAILED;

	CopyMemory((PUCHAR)pack->Buffer + pack->Length, data, size);

	pack->Length += size;

	return PACKAGE_SUCCESS;
}

INT AddBytesToPackage(pPackage pack, PBYTE data, SIZE_T size) {

	if (!pack)
		return PACKAGE_DOES_NOT_EXIST;

	if (!data)
		return PACKAGE_DATA_DOES_NOT_EXIST;

	if (!size || size < 1)
		return PACKAGE_UNEXPECTED_DATA_SIZE;

	pack->Buffer = LocalReAlloc(pack->Buffer, 
		pack->Length + size, 
		LMEM_MOVEABLE | LMEM_ZEROINIT
	);
	
	if (!pack->Buffer)
		return PACKAGE_BUFFER_REALLOC_FAILED;

	CopyMemory((PUCHAR)pack->Buffer + pack->Length, data, size);
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
	pPackage pkg = g_ImplantData.packages;

	if (!pack)
		return PACKAGE_DOES_NOT_EXIST;

	while (pkg)
	{
		if (pack == pkg) {
			return PACKAGE_HAS_NOT_BEEN_SENT_TO_SERVER;
		}
		pkg = pkg->Next;
	}

	if (!pack->Buffer)
		return PACKAGE_BUFFER_DOES_NOT_EXIST;

	SetMemory(pack->Buffer, 0, pack->Length);
	LocalFree(pack->Buffer);
	pack->Buffer = NULL;

	SetMemory(pack->Buffer, 0, pack->Length);
	LocalFree(pack->Buffer);
	pack->Buffer = NULL;

	return PACKAGE_SUCCESS;
}

LPSTR ErrorToString(INT error) {
	LPSTR error[100];
	switch (error)
	{
	case   PACKAGE_FALIURE_GENERIC:
		sprintf_s(error,sizeof(error), "GENERIC PACKAGE FAILURE\n");
		break;
	case PACKAGE_DOES_NOT_EXIST:
		sprintf_s(error, sizeof(error), "PACKAGE_DOES_NOT_EXIST\n");
		break;
	case PACKAGE_DATA_DOES_NOT_EXIST:
		sprintf_s(error, sizeof(error), "PACKAGE_DATA_DOES_NOT_EXIST\n");
		break;
	case PACKAGE_UNEXPECTED_DATA_SIZE:
		sprintf_s(error, sizeof(error), "PACKAGE_UNEXPECTED_DATA_SIZE\n");
		break;
	case PACKAGE_BUFFER_REALLOC_FAILED:
		sprintf_s(error, sizeof(error), "PACKAGE_BUFFER_REALLOC_FAILED\n");
		break;
	case PACKAGE_BUFFER_DOES_NOT_EXIST:
		sprintf_s(error, sizeof(error), "PACKAGE_BUFFER_DOES_NOT_EXIST\n");
		break;
	case PACKAGE_POINTER_DOES_NOT_EXIST:
		sprintf_s(error, sizeof(error), "PACKAGE_POINTER_DOES_NOT_EXIST\n");
		break;
	case PACKAGE_HAS_NOT_BEEN_SENT_TO_SERVER:
		sprintf_s(error, sizeof(error), "PACKAGE_HAS_NOT_BEEN_SENT_TO_SERVER\n");
		break;
	}
	return error;
}