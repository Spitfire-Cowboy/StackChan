/*
SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
SPDX-License-Identifier: MIT
*/

package wsproto

import "fmt"

type DeviceControlAction string

const (
	DeviceControlActionSleep    DeviceControlAction = "sleep"
	DeviceControlActionWake     DeviceControlAction = "wake"
	DeviceControlActionPowerOff DeviceControlAction = "powerOff"
)

type DeviceControlPayload struct {
	Type   string              `json:"type"`
	Action DeviceControlAction `json:"action"`
}

func CreateDeviceControlPacket(action DeviceControlAction) ([]byte, error) {
	switch action {
	case DeviceControlActionSleep, DeviceControlActionWake, DeviceControlActionPowerOff:
	default:
		return nil, fmt.Errorf("unsupported device control action: %q", action)
	}

	return createTextMessagePacket(DeviceControlPayload{
		Type:   "deviceControl",
		Action: action,
	})
}

func CreateSleepPacket() ([]byte, error) {
	return CreateDeviceControlPacket(DeviceControlActionSleep)
}

func CreateWakePacket() ([]byte, error) {
	return CreateDeviceControlPacket(DeviceControlActionWake)
}

func CreatePowerOffPacket() ([]byte, error) {
	return CreateDeviceControlPacket(DeviceControlActionPowerOff)
}
