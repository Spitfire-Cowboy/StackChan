/*
SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
SPDX-License-Identifier: MIT
*/

package wsproto

import (
	"encoding/json"
	"strings"
	"testing"
)

func decodeDeviceControlPacket(t *testing.T, packet []byte) DeviceControlPayload {
	t.Helper()

	if len(packet) < 5 {
		t.Fatalf("packet too short: %d", len(packet))
	}
	if packet[0] != TextMessageType {
		t.Fatalf("expected text message type %d, got %d", TextMessageType, packet[0])
	}

	var payload DeviceControlPayload
	if err := json.Unmarshal(packet[5:], &payload); err != nil {
		t.Fatalf("failed to decode packet payload: %v", err)
	}
	return payload
}

func TestCreateDeviceControlPacket(t *testing.T) {
	packet, err := CreateDeviceControlPacket(DeviceControlActionWake)
	if err != nil {
		t.Fatalf("CreateDeviceControlPacket returned error: %v", err)
	}

	payload := decodeDeviceControlPacket(t, packet)
	if payload.Type != "deviceControl" {
		t.Fatalf("expected deviceControl type, got %q", payload.Type)
	}
	if payload.Action != DeviceControlActionWake {
		t.Fatalf("expected wake action, got %q", payload.Action)
	}
}

func TestCreateDeviceControlPacketRejectsUnknownAction(t *testing.T) {
	_, err := CreateDeviceControlPacket(DeviceControlAction("hibernate"))
	if err == nil {
		t.Fatal("expected invalid action error")
	}
	if !strings.Contains(err.Error(), "unsupported device control action") {
		t.Fatalf("unexpected error: %v", err)
	}
}

func TestCreateSleepPacket(t *testing.T) {
	packet, err := CreateSleepPacket()
	if err != nil {
		t.Fatalf("CreateSleepPacket returned error: %v", err)
	}

	payload := decodeDeviceControlPacket(t, packet)
	if payload.Action != DeviceControlActionSleep {
		t.Fatalf("expected sleep action, got %q", payload.Action)
	}
}

func TestCreateWakePacket(t *testing.T) {
	packet, err := CreateWakePacket()
	if err != nil {
		t.Fatalf("CreateWakePacket returned error: %v", err)
	}

	payload := decodeDeviceControlPacket(t, packet)
	if payload.Action != DeviceControlActionWake {
		t.Fatalf("expected wake action, got %q", payload.Action)
	}
}

func TestCreatePowerOffPacket(t *testing.T) {
	packet, err := CreatePowerOffPacket()
	if err != nil {
		t.Fatalf("CreatePowerOffPacket returned error: %v", err)
	}

	payload := decodeDeviceControlPacket(t, packet)
	if payload.Action != DeviceControlActionPowerOff {
		t.Fatalf("expected powerOff action, got %q", payload.Action)
	}
}
