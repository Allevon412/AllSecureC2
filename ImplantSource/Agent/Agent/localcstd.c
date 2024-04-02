
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