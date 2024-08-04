package Common

import (
	"golang.org/x/text/encoding/unicode"
	"log"
	"strings"
	"time"
)

func CheckIfKeyIVIsEqual(CurrKey []byte, CurrIV []byte, NewKey []byte, NewIV []byte) bool {

	for i, _ := range CurrKey {
		if CurrKey[i] != NewKey[i] {
			return false
		}
	}
	for i, _ := range CurrIV {
		if CurrIV[i] != NewIV[i] {
			return false
		}
	}

	return true
}

func CheckIfAgentIsAlive(Agent *Implant) bool {

	if Agent.Alive && Agent.Context.LastCheckin != (time.Time{}) && Agent.Context.Sleep_delay != 0 {
		if Agent.Context.LastCheckin.Add(time.Duration(float32(Agent.Context.Sleep_delay)*1.5) * time.Second).Before(time.Now()) {
			Agent.Alive = false
			log.Println("[info] agent [", Agent.Context.Agent_name, "] has not checked in for ", float32(Agent.Context.Sleep_delay)*1.5, " seconds")
			return false
		}
	}
	return true
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
