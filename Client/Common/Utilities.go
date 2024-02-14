package Common

import (
	"encoding/base64"
	"fmt"
	"strings"
)

func ParseBool(s string) (bool, error) {
	switch strings.ToLower(s) {
	case "true", "1", "yes", "y":
		return true, nil
	case "false", "0", "no", "n":
		return false, nil
	default:
		return false, fmt.Errorf("invalid boolean value: %s", s)
	}
}

func Base64Decode(encodedstr string) []byte {
	dst := make([]byte, base64.StdEncoding.DecodedLen(len(encodedstr)))
	_, err := base64.StdEncoding.Decode(dst, []byte(encodedstr))
	if err != nil {
		fmt.Println("decode error:", err)
		return []byte{}
	}
	return dst
}

func RemoveNullBytes(data []byte) []byte {
	var result []byte

	for _, b := range data {
		if b != 0x00 {
			result = append(result, b)
		}
	}

	return result
}
