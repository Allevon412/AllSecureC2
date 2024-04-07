
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