package Utility

import (
	"AllSecure/TeamServer/Common/Types"
	"encoding/json"
	"errors"
	"golang.org/x/text/encoding/unicode"
	"log"
	"os"
	"regexp"
	"strconv"
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

func ParseWorkingHours(WorkingHours string) (int32, error) {
	/*
	 * The working hours are packed in a uint32
	 * X: enabled or not
	 * A: start hour
	 * B: start minute
	 * C: end hour
	 * D: end minute
	 * Minutes = 6 bits (0-63) 111111 = 63 | max = 111100 = 60
	 * Hours = 5 bits (0-31) 11111 = 31 | max = 11000 = 24
	 *          XAAAAABBBBBBCCCCCDDDDDD
	 * 00000000010000011111100000111111
	 * ------------32 bits-------------
	 */
	var (
		IntWorkingHours int32 = 0
	)
	if WorkingHours != "" {
		match, err := regexp.MatchString("^[12]?[0-9]:[0-6][0-9]-[12]?[0-9]:[0-6][0-9]$", WorkingHours)
		if err != nil || match == false {
			return IntWorkingHours, errors.New("[error] failed to parse working hours: Invalid format: format should be : 8:00-17:00")
		}

		StartAndEnd := strings.Split(WorkingHours, "-")
		StartHoursAndMinutes := strings.Split(StartAndEnd[0], ":")
		EndHoursAndMinutes := strings.Split(StartAndEnd[1], ":")

		startHour, _ := strconv.Atoi(StartHoursAndMinutes[0])
		startMinute, _ := strconv.Atoi(StartHoursAndMinutes[1])
		endHour, _ := strconv.Atoi(EndHoursAndMinutes[0])
		endMinute, _ := strconv.Atoi(EndHoursAndMinutes[1])

		if startHour < 0 || startHour > 24 || endHour < 0 || endHour > 24 || startMinute < 0 ||
			startMinute > 60 || endMinute < 0 || endMinute > 60 {
			return IntWorkingHours, errors.New("[Error] failed to parse the working hours from config. Time not within expected ranges")
		}
		if endHour < startHour || (startHour == endHour && endMinute <= startMinute) {
			return IntWorkingHours, errors.New("[error] failed to parse the working hours from config. End time can't be before or equal to the start time")
		}
		IntWorkingHours |= 1 << 22                               //sets the enabled bit
		IntWorkingHours |= (int32(startHour) & 0b011111) << 17   // sets the start hour
		IntWorkingHours |= (int32(startMinute) & 0b111111) << 11 // sets the start minute
		IntWorkingHours |= (int32(endHour) & 0b011111) << 6      // sets the end hour
		IntWorkingHours |= (int32(endMinute) & 0b111111)         // sets the end minute

	}

	return IntWorkingHours, nil
}

func ParseConfig(FilePath, FileName string, Config any) (any, error) {
	var (
		val any
		ok  bool
	)
	content, err := os.ReadFile(FilePath + FileName)
	if err != nil {
		log.Fatalln("[error] Error opening file", err)
	}
	switch FileName {
	case "AllSecure.Config":
		if val, ok = Config.(*Types.TSConfig); ok {
			err = json.Unmarshal(content, &val)
			if err != nil {
				log.Println("[error] Error unmarshalling JSON", err)
				return nil, err
			}
			log.Println("[info] TeamServer configuration file parsed successfully: ", val)
		}
		break
	case "Implant.Config":
		if val, ok = Config.(*Types.ImplantConfig); ok {
			err = json.Unmarshal(content, &val)
			if err != nil {
				log.Println("[error] Error unmarshalling JSON, Implant config", err)
				return nil, err
			}
			log.Println("[info] Implant configuration file parsed successfully: ", val)
		}
		break

	case "Builder.Config":
		if val, ok = Config.(*Types.BuilderConfig); ok {
			err = json.Unmarshal(content, &val)
			if err != nil {
				log.Println("[error] Error unmarshalling JSON, builder config.", err)
				return nil, err
			}
			log.Println("[info] Builder configuration file parsed successfully: ", val)
		}
		break

	}

	return val, nil
}
