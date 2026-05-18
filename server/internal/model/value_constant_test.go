/*
SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
SPDX-License-Identifier: MIT
*/

package model

import (
	"encoding/json"
	"testing"
)

func TestDefaultDanceDataIsValidJSON(t *testing.T) {
	t.Parallel()

	var decoded []DanceData
	if err := json.Unmarshal([]byte(DefaultDanceData), &decoded); err != nil {
		t.Fatalf("default dance data must stay valid JSON: %v", err)
	}

	if len(decoded) == 0 {
		t.Fatal("default dance data must contain at least one keyframe")
	}

	first := decoded[0]
	if first.LeftEye.Weight != 100 || first.RightEye.Weight != 100 {
		t.Fatalf("unexpected neutral eye weights in first keyframe: %+v", first)
	}
	if first.PitchServo.Speed == 0 || first.DurationMs == 0 {
		t.Fatalf("first keyframe should include servo speed and duration: %+v", first)
	}
}
