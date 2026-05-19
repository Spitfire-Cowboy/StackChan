/*
SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
SPDX-License-Identifier: MIT
*/

package device

import (
	"testing"

	"stackChan/internal/wsproto"
)

func TestBuildDeviceControlAction(t *testing.T) {
	tests := []struct {
		name     string
		input    string
		expected wsproto.DeviceControlAction
		ok       bool
	}{
		{name: "sleep", input: "sleep", expected: wsproto.DeviceControlActionSleep, ok: true},
		{name: "wake", input: " Wake ", expected: wsproto.DeviceControlActionWake, ok: true},
		{name: "power off", input: "powerOff", expected: wsproto.DeviceControlActionPowerOff, ok: true},
		{name: "power off underscore", input: "power_off", expected: wsproto.DeviceControlActionPowerOff, ok: true},
		{name: "shutdown alias", input: "shutdown", expected: wsproto.DeviceControlActionPowerOff, ok: true},
		{name: "invalid", input: "hibernate", expected: "", ok: false},
	}

	for _, test := range tests {
		t.Run(test.name, func(t *testing.T) {
			got, ok := buildDeviceControlAction(test.input)
			if ok != test.ok {
				t.Fatalf("expected ok=%v, got %v", test.ok, ok)
			}
			if got != test.expected {
				t.Fatalf("expected action %q, got %q", test.expected, got)
			}
		})
	}
}
