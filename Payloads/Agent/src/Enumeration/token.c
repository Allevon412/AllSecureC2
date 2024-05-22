#include "token.h"

HANDLE TokenCurrentHandle(
    pAgent agent
) {
    HANDLE   Token = NULL;
    NTSTATUS NtStatus = STATUS_UNSUCCESSFUL;

    if (!NT_SUCCESS(NtStatus = agent->pNtOpenThreadToken(NtCurrentThread(), TOKEN_QUERY, TRUE, &Token)))
    {
        if (NtStatus != STATUS_NO_TOKEN)
        {
            //PRINTF("NtOpenThreadToken: Failed:[%08x : %ld]\n", NtStatus, Instance->Win32.RtlNtStatusToDosError(NtStatus));
            //NtSetLastError(Instance->Win32.RtlNtStatusToDosError(NtStatus));
            return NULL;
        }

        if (!NT_SUCCESS(NtStatus = agent->pNtOpenProcessToken(NtCurrentProcess(), TOKEN_QUERY, &Token)))
        {
            //PRINTF("NtOpenProcessToken: Failed:[%08x : %ld]\n", NtStatus, Instance->Win32.RtlNtStatusToDosError(NtStatus));
            //NtSetLastError(Instance->Win32.RtlNtStatusToDosError(NtStatus));
            return NULL;
        }
    }

    return Token;
}

BOOL TokenElevated( IN HANDLE Token, pAgent agent) {
    TOKEN_ELEVATION Data = { 0 };
    DWORD           Size = sizeof(TOKEN_ELEVATION);
    BOOL            Admin = FALSE;

    if (NT_SUCCESS(agent->pNtQueryInformationToken(Token, TokenElevation, &Data, Size, &Size))) {
        Admin = Data.TokenIsElevated;
    }

    return Admin;
}

