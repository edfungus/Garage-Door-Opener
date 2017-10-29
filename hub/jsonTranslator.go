package main

import (
	"encoding/json"

	"github.com/edfungus/eevee"
)

// messagePayload is the expected json payload structure for the input and output
type messagePayload struct {
	ID int `json:"id,omitempty"`
}

// JsonTranslator in/outputs same json with known structure
type JsonTranslator struct{}

func NewJsonTranslator() *JsonTranslator {
	return &JsonTranslator{}
}

func (jt *JsonTranslator) GetID(rawMessage []byte) (eevee.MessageID, error) {
	if len(rawMessage) == 0 {
		rawMessage = []byte("{}")
	}
	var message messagePayload
	err := json.Unmarshal(rawMessage, &message)
	if err != nil {
		log.Errorf("Could not get id with error: %v", err)
		return nil, err
	}
	if message.ID == 0 {
		return eevee.NoMessageID, nil
	}
	return message.ID, nil
}

func (jt *JsonTranslator) SetID(rawMessage []byte, id eevee.MessageID) ([]byte, error) {
	var message messagePayload
	err := json.Unmarshal(rawMessage, &message)
	if err != nil {
		log.Errorf("Could not set id with error: %v", err)
		return nil, err
	}
	message.ID = id.(int)
	return json.Marshal(message)
}

func (jt *JsonTranslator) TranslateOut(rawMessage []byte) (outRawMessage []byte) {
	if len(rawMessage) == 0 {
		return []byte("{}")
	}
	return rawMessage
}
