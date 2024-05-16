package Common

import "fmt"

/*
	[ dwMajorVersion ] 4 bytes
	[ dwMinorVersion ] 4 bytes
	[ dwProductType  ] 4 bytes
	[ dwServicePackMajor ] 4 bytes
	[ dwBuildNumber ] 4 bytes
*/

func GetWindowsVersion(Os_Info [5]uint32) string {
	var (
		version                               string
		major, minor, build, servicePackMajor uint32
	)
	major = Os_Info[0]
	minor = Os_Info[1]
	servicePackMajor = Os_Info[3]
	build = Os_Info[4]

	switch major {
	case 10:
		if minor == 0 {
			switch build {
			case 10240:
				version = "Windows 10"
			case 10586:
				version = "Windows 10, version 1511"
			case 14393:
				version = "Windows 10, version 1607"
			case 15063:
				version = "Windows 10, version 1703"
			case 16299:
				version = "Windows 10, version 1709"
			case 17134:
				version = "Windows 10, version 1803"
			case 17763:
				version = "Windows 10, version 1809"
			case 18362:
				version = "Windows 10, version 1903"
			case 18363:
				version = "Windows 10, version 1909"
			case 19041:
				version = "Windows 10, version 2004"
			case 19042:
				version = "Windows 10, version 20H2"
			case 19043:
				version = "Windows 10, version 21H1"
			case 19044:
				version = "Windows 10, version 21H2"
			default:
				version = fmt.Sprintf("Windows 10, build %d", build)
			}
		}
	case 6:
		switch minor {
		case 3:
			version = "Windows 8.1"
		case 2:
			version = "Windows 8"
		case 1:
			if servicePackMajor == 1 {
				version = "Windows 7 SP1"
			} else {
				version = "Windows 7"
			}
		case 0:
			if servicePackMajor == 1 {
				version = "Windows Vista SP1"
			} else if servicePackMajor == 2 {
				version = "Windows Vista SP2"
			} else {
				version = "Windows Vista"
			}
		}
	case 5:
		switch minor {
		case 2:
			if servicePackMajor == 2 {
				version = "Windows XP 64-Bit Edition"
			} else {
				version = "Windows Server 2003"
			}
		case 1:
			version = "Windows XP"
		}
	default:
		version = fmt.Sprintf("Unknown Windows version: %d.%d", major, minor)
	}

	return version
}
