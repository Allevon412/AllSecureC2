#include "localcstd.h"

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