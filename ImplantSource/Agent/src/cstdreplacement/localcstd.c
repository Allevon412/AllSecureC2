#include "../../headers/cstdreplacement/localcstd.h"

SIZE_T StringLengthA(LPCSTR String) {
	LPCSTR String2;

	if (!String)
		return 0;

	for (String2 = String; *String2; ++String2);

	return (String2 - String);
}

SIZE_T StringLengthW(LPCWSTR String) {
	LPCWSTR String2;

	if (!String)
		return 0;

	for (String2 = String; *String2; ++String2);

	return (String2 - String);
}

LPSTR ToLowerA(LPSTR String) {
	if (String == NULL) return NULL;

	// Allocate memory for the new string (+1 for the null terminator)
	LPSTR lowerStr = (LPSTR)agent->apis->pLocalAlloc(LPTR, (StringLengthA(String) + 1));
	if (lowerStr == NULL) return NULL; // Check for allocation failure

	// Convert each character to lowercase and copy it
	for (int i = 0; String[i] != '\0'; i++) {
		if (String[i] >= 'A' && String[i] <= 'Z') {
			lowerStr[i] = String[i] + 32; // Convert to lowercase
		}
		else {
			lowerStr[i] = String[i]; // Copy character as is
		}
	}

	// Null-terminate the new string
	lowerStr[StringLengthA(String)] = '\0';

	return lowerStr;
}

LPWSTR ToLowerW(LPWSTR String) {
	if (String == NULL) return NULL;

	// Allocate memory for the new string (+1 for the null terminator)
	LPWSTR lowerStr = (LPWSTR)agent->apis->pLocalAlloc(LPTR, ((StringLengthW(String) + 1) * sizeof(WCHAR)));
	if (lowerStr == NULL) return NULL; // Check for allocation failure

	// Convert each character to lowercase and copy it
	for (int i = 0; String[i] != '\0'; i++) {
		if (String[i] >= 'A' && String[i] <= 'Z') {
			lowerStr[i] = String[i] + 32; // Convert to lowercase
		}
		else {
			lowerStr[i] = String[i]; // Copy character as is
		}
	}

	// Null-terminate the new string
	lowerStr[StringLengthW(String)] = '\0';

	return lowerStr;
}

void MemoryCopy(void* dest, const void* src, size_t n) {
	char* csrc = (char*)src;
	char* cdest = (char*)dest;

	// Copying data byte by byte
	for (size_t i = 0; i < n; i++)
		cdest[i] = csrc[i];
}

int GetLPSTRArraySize(LPSTR* array) {
	int count = 0;
	while (array[count] != NULL) {
		count++;
	}
	return count;
}

UINT32 bswap32(UINT32 x) {
	return ((x << 24) & 0xff000000) |
		((x << 8) & 0x00ff0000) |
		((x >> 8) & 0x0000ff00) |
		((x >> 24) & 0x000000ff);
}

LPSTR generateRandomString(size_t length) {
	// Define the character set to use (you can modify this to include more characters)
	const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	size_t charsetSize = StringLengthA(charset); // Subtract 1 to avoid the null terminator
	LPSTR str = (LPSTR)agent->apis->pLocalAlloc(LPTR, length + 1);

	ULONG Seed = 0;
	Seed = agent->apis->pNtGetTickCount();


	// Generate the random string
	for (size_t i = 0; i < length; ++i) {
		Seed = agent->apis->pRtlRandomEx(&Seed);
		int key = Seed % charsetSize;
		str[i] = charset[key];
	}

	// Null-terminate the string
	str[length] = '\0';
	return str;
}
LPWSTR generateRandomStringW(size_t length) {
	// Define the character set to use (you can modify this to include more characters)
	const LPWSTR charset = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	size_t charsetSize = StringLengthW(charset); 
	LPWSTR str = (LPSTR)agent->apis->pLocalAlloc(LPTR, (length + 1) * sizeof(WCHAR));

	// Seed the random number generator
	ULONG Seed = 0;
	Seed = agent->apis->pNtGetTickCount();

	// Generate the random string
	for (size_t i = 0; i < length; ++i) {
		Seed = agent->apis->pRtlRandomEx(&Seed);
		int key = Seed % charsetSize;
		str[i] = charset[key];
	}

	// Null-terminate the string
	str[length] = L'\0';
	return str;
}

UINT64 GetUnixTime()
{
	FILETIME ft;
	LARGE_INTEGER li;

	agent->apis->pGetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;

	const UINT64 EPOCH_DIFFERENCE = 116444736000000000ULL;
	UINT64 time = (li.QuadPart - EPOCH_DIFFERENCE) / 10000000ULL;
	
	return time;
}