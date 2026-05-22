/*
SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
SPDX-License-Identifier: MIT
*/

package device

import (
	"strings"

	"stackChan/internal/wsproto"
)

func buildDeviceControlAction(action string) (wsproto.DeviceControlAction, bool) {
	switch strings.TrimSpace(strings.ToLower(action)) {
	case "sleep":
		return wsproto.DeviceControlActionSleep, true
	case "wake":
		return wsproto.DeviceControlActionWake, true
	case "poweroff", "power_off", "shutdown":
		return wsproto.DeviceControlActionPowerOff, true
	default:
		return "", false
	}
}
