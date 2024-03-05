#include "enum.h"

RTL_OSVERSIONINFOEXW GetOperatingSystemFunc() {

	t_RtlGetVersion pRtlGetVersion = GetProcAddress(GetModuleHandleA("Ntdll.dll"), "RtlGetVersion");

	RTL_OSVERSIONINFOEXW lpVersionInformation = { 0 };
	NTSTATUS status = 0;
	status = pRtlGetVersion(&lpVersionInformation);
	if (status != NTSUCCESS) {
		printf("[error] attempting to retrieve the operating system versions. [%08X]", status);
		return lpVersionInformation;
	}
	
	printf("operating system major version: [%d]\noperating system minor version: [%d]\noperating system build number: [%d]\n" \
		"operating system service pack major: [%d]\noperating system service pack minor: [%d]\n", lpVersionInformation.dwMajorVersion, lpVersionInformation.dwMinorVersion,
		lpVersionInformation.dwBuildNumber, lpVersionInformation.wServicePackMajor, lpVersionInformation.wServicePackMinor);

	
	return lpVersionInformation;
}