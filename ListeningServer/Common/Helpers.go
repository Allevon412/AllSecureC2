package Common

import (
	"log"
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

	if Agent.Alive && Agent.LastCheckin != (time.Time{}) && Agent.Context.Sleep_delay != 0 {
		if Agent.LastCheckin.Add(time.Duration(float32(Agent.Context.Sleep_delay)*1.5) * time.Second).Before(time.Now()) {
			Agent.Alive = false
			log.Println("[info] agent [", Agent.Context.Agent_name, "] has not checked in for ", float32(Agent.Context.Sleep_delay)*1.5, " seconds")
			return false
		}
	}
	return true
}
