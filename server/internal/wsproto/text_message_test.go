/*
SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
SPDX-License-Identifier: MIT
*/

package wsproto

import (
	"encoding/binary"
	"encoding/json"
	"testing"
)

func decodeTextMessagePacket(t *testing.T, packet []byte) TextMessagePayload {
	t.Helper()

	if len(packet) < 5 {
		t.Fatalf("expected framed packet, got %d bytes", len(packet))
	}
	if packet[0] != TextMessageType {
		t.Fatalf("expected TextMessage type, got %d", packet[0])
	}

	payloadLen := binary.BigEndian.Uint32(packet[1:5])
	if int(payloadLen) != len(packet)-5 {
		t.Fatalf("length prefix mismatch: got %d, payload bytes %d", payloadLen, len(packet)-5)
	}

	var payload TextMessagePayload
	if err := json.Unmarshal(packet[5:], &payload); err != nil {
		t.Fatalf("unmarshal payload: %v", err)
	}

	return payload
}

func TestCreateTextMessagePacket(t *testing.T) {
	packet, err := CreateTextMessagePacket("Pro777", "Ship it.")
	if err != nil {
		t.Fatalf("CreateTextMessagePacket returned error: %v", err)
	}

	payload := decodeTextMessagePacket(t, packet)
	if payload.Type != "" {
		t.Fatalf("expected empty type for legacy speech packet, got %q", payload.Type)
	}
	if payload.Name != "Pro777" {
		t.Fatalf("expected name Pro777, got %q", payload.Name)
	}
	if payload.Content != "Ship it." {
		t.Fatalf("expected content %q, got %q", "Ship it.", payload.Content)
	}
	if payload.Title != "" || len(payload.Lines) != 0 || payload.Accent != "" || payload.TimeoutMs != 0 {
		t.Fatalf("expected speech payload fields to stay unset, got %#v", payload)
	}
}

func TestCreateDisplayCardPacket(t *testing.T) {
	packet, err := CreateDisplayCardPacket("SHIP RECEIPTS", []string{"Pro777", "Streak 3d"}, "#123456", 9000)
	if err != nil {
		t.Fatalf("CreateDisplayCardPacket returned error: %v", err)
	}

	payload := decodeTextMessagePacket(t, packet)
	if payload.Type != "displayCard" {
		t.Fatalf("expected displayCard type, got %q", payload.Type)
	}
	if payload.Title != "SHIP RECEIPTS" {
		t.Fatalf("expected title %q, got %q", "SHIP RECEIPTS", payload.Title)
	}
	if len(payload.Lines) != 2 || payload.Lines[0] != "Pro777" || payload.Lines[1] != "Streak 3d" {
		t.Fatalf("unexpected lines: %#v", payload.Lines)
	}
	if payload.Accent != "#123456" {
		t.Fatalf("expected accent #123456, got %q", payload.Accent)
	}
	if payload.TimeoutMs != 9000 {
		t.Fatalf("expected timeout 9000, got %d", payload.TimeoutMs)
	}
}

func TestCreateDisplayCardPacketDefaults(t *testing.T) {
	packet, err := CreateDisplayCardPacket("STATUS", nil, "", 0)
	if err != nil {
		t.Fatalf("CreateDisplayCardPacket returned error: %v", err)
	}

	payload := decodeTextMessagePacket(t, packet)
	if payload.Accent != defaultDisplayCardAccent {
		t.Fatalf("expected default accent %q, got %q", defaultDisplayCardAccent, payload.Accent)
	}
	if payload.TimeoutMs != defaultDisplayCardTimeoutMs {
		t.Fatalf("expected default timeout %d, got %d", defaultDisplayCardTimeoutMs, payload.TimeoutMs)
	}
	if payload.Lines != nil {
		t.Fatalf("expected nil lines to stay omitted, got %#v", payload.Lines)
	}
}

func TestFrameMessage(t *testing.T) {
	packet := FrameMessage(0x42, []byte("abc"))
	if len(packet) != 8 {
		t.Fatalf("expected 8 bytes, got %d", len(packet))
	}
	if packet[0] != 0x42 {
		t.Fatalf("expected type 0x42, got 0x%x", packet[0])
	}
	if got := binary.BigEndian.Uint32(packet[1:5]); got != 3 {
		t.Fatalf("expected payload len 3, got %d", got)
	}
	if string(packet[5:]) != "abc" {
		t.Fatalf("expected payload abc, got %q", string(packet[5:]))
	}
}
