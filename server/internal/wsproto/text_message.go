/*
SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
SPDX-License-Identifier: MIT
*/

package wsproto

import (
	"encoding/binary"
	"encoding/json"
)

const (
	TextMessageType             byte   = 0x07
	defaultDisplayCardAccent    string = "#FFDF9A"
	defaultDisplayCardTimeoutMs uint32 = 4000
)

type TextMessagePayload struct {
	Type      string   `json:"type,omitempty"`
	Name      string   `json:"name,omitempty"`
	Content   string   `json:"content,omitempty"`
	Title     string   `json:"title,omitempty"`
	Lines     []string `json:"lines,omitempty"`
	Accent    string   `json:"accent,omitempty"`
	TimeoutMs uint32   `json:"timeoutMs,omitempty"`
	Sticky    bool     `json:"sticky,omitempty"`
	Clear     bool     `json:"clear,omitempty"`
}

func CreateTextMessagePacket(name, content string) ([]byte, error) {
	return createTextMessagePacket(TextMessagePayload{
		Name:    name,
		Content: content,
	})
}

func CreateDisplayCardPacket(title string, lines []string, accent string, timeoutMs uint32, sticky bool, clear bool) ([]byte, error) {
	if accent == "" {
		accent = defaultDisplayCardAccent
	}
	if timeoutMs == 0 && !sticky && !clear {
		timeoutMs = defaultDisplayCardTimeoutMs
	}

	return createTextMessagePacket(TextMessagePayload{
		Type:      "displayCard",
		Title:     title,
		Lines:     append([]string(nil), lines...),
		Accent:    accent,
		TimeoutMs: timeoutMs,
		Sticky:    sticky,
		Clear:     clear,
	})
}

func CreateStickyDisplayCardPacket(title string, lines []string, accent string) ([]byte, error) {
	return CreateDisplayCardPacket(title, lines, accent, 0, true, false)
}

func CreateClearDisplayCardPacket() ([]byte, error) {
	return CreateDisplayCardPacket("", nil, "", 0, false, true)
}

func createTextMessagePacket(payload any) ([]byte, error) {
	body, err := json.Marshal(payload)
	if err != nil {
		return nil, err
	}

	return FrameMessage(TextMessageType, body), nil
}

func FrameMessage(msgType byte, data []byte) []byte {
	dataLen := len(data)
	msg := make([]byte, 1+4+dataLen)
	msg[0] = msgType
	binary.BigEndian.PutUint32(msg[1:5], uint32(dataLen))
	copy(msg[5:], data)
	return msg
}
