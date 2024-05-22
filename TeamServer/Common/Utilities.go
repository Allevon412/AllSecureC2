package Common

import (
	"golang.org/x/text/encoding/unicode"
	"log"
	"strings"
)

func RemoveNullBytes(data []byte) []byte {
	var result []byte

	for _, b := range data {
		if b != 0x00 {
			result = append(result, b)
		}
	}

	return result
}

func EncodeUTF16(s string) []byte {
	var err error
	// c strings are null terminated

	if strings.HasSuffix(s, "\x00") == false {
		s += "\x00"
	}
	uni := unicode.UTF16(unicode.LittleEndian, unicode.IgnoreBOM)
	encoded, err := uni.NewEncoder().String(s)
	if err != nil {
		log.Println("[error] attempting to encode string to utf16", err)
		return []byte{}
	}
	return []byte(encoded)
}

func EncodeUTF8(s string) []byte {
	//c strings are null terminated
	if strings.HasSuffix(s, "\x00") == false {
		s += "\x00"
	}

	return []byte(s)
}
