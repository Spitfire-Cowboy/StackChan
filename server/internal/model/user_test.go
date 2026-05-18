/*
SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
SPDX-License-Identifier: MIT
*/

package model

import (
	"encoding/json"
	"testing"
)

func TestRemoteRegisterResponseJSONMapping(t *testing.T) {
	t.Parallel()

	payload := []byte(`{
		"status": {
			"code": "ok",
			"message": "created"
		},
		"response": {
			"uid": 7,
			"username": "pro777",
			"userslug": "pro777",
			"email": "pro777@example.com",
			"email:confirmed": 1,
			"joindate": 1747094400000,
			"lastonline": 1747094405000,
			"picture": null,
			"icon:bgColor": "#ffaa00",
			"fullname": null,
			"displayname": "Pro777",
			"icon:text": "P7",
			"status": "online"
		}
	}`)

	var decoded RemoteRegisterResp
	if err := json.Unmarshal(payload, &decoded); err != nil {
		t.Fatalf("unmarshal remote register response: %v", err)
	}

	if decoded.Status.Code != "ok" {
		t.Fatalf("unexpected status code: %q", decoded.Status.Code)
	}
	if decoded.RegistrationResponse.Username != "pro777" {
		t.Fatalf("unexpected username: %q", decoded.RegistrationResponse.Username)
	}
	if decoded.RegistrationResponse.IconBgColor != "#ffaa00" {
		t.Fatalf("unexpected icon background color: %q", decoded.RegistrationResponse.IconBgColor)
	}
	if decoded.RegistrationResponse.UserStatus != "online" {
		t.Fatalf("unexpected user status: %q", decoded.RegistrationResponse.UserStatus)
	}
}
