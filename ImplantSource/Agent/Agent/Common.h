#pragma once
#include <Windows.h>
#include <winternl.h>
#include <winhttp.h>


#define AES_BLOCKLEN 16
#define NTSUCCESS 0
#define UNLEN 256

//HTTP APIS
//WINHTTP APIS
typedef HINTERNET(WINAPI* t_WinHttpOpen)(
	LPCWSTR pszAgentW,
	DWORD   dwAccessType,
	LPCWSTR pszProxyW,
	LPCWSTR pszProxyBypassW,
	DWORD   dwFlags
	);

typedef HINTERNET(WINAPI* t_WinHttpConnect)(
	HINTERNET     hSession,
	LPCWSTR       pswzServerName,
	INTERNET_PORT nServerPort,
	DWORD         dwReserved
	);



typedef HINTERNET(WINAPI* t_WinHttpOpenRequest)(
	HINTERNET hConnect,
	LPCWSTR   pwszVerb,
	LPCWSTR   pwszObjectName,
	LPCWSTR   pwszVersion,
	LPCWSTR   pwszReferrer,
	LPCWSTR* ppwszAcceptTypes,
	DWORD     dwFlags
	);

typedef BOOL(WINAPI* t_WinHttpSetOption)(
	HINTERNET hInternet,
	DWORD     dwOption,
	LPVOID    lpBuffer,
	DWORD     dwBufferLength
	);

typedef BOOL(WINAPI* t_WinHttpAddRequestHeaders)(
	HINTERNET hRequest,
	LPCWSTR   pwszHeaders,
	DWORD     dwHeadersLength,
	DWORD     dwModifiers
	);


typedef BOOL(WINAPI* t_WinHttpSendRequest)(
HINTERNET hRequest,
	LPCWSTR   pwszHeaders,
	DWORD     dwHeadersLength,
	LPVOID    lpOptional,
	DWORD     dwOptionalLength,
	DWORD     dwTotalLength,
	DWORD_PTR  dwContext
	);

typedef BOOL(WINAPI* t_WinHttpReceiveResponse)(
	HINTERNET hRequest,
	LPVOID    lpReserved
	);

typedef BOOL(WINAPI* t_WinHttpQueryHeaders)(

HINTERNET hRequest,
	DWORD     dwInfoLevel,
	LPCWSTR   pwszName,
	LPVOID    lpBuffer,
	LPDWORD   lpdwBufferLength,
	LPDWORD   lpdwIndex
	);




// enumeration APIS
//NTDLL APIS
typedef NTSTATUS(NTAPI* t_RtlGetVersion)(
	PRTL_OSVERSIONINFOW lpVersionInformation
	);

//ADVAPI APIS
typedef BOOL(WINAPI* t_GetUserNameA)(
	LPSTR   lpBuffer,
	LPDWORD pcbBuffer
	);



//KERNEL32 APIS
typedef BOOL(WINAPI* t_GetComputerNameExA)(
	COMPUTER_NAME_FORMAT NameType,
	LPSTR                lpBuffer,
	LPDWORD              nSize
	);

//USER32 APIS
typedef int (WINAPI* t_GetSystemMetrics)(
	int nIndex
	);