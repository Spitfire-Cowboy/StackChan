/*
SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
SPDX-License-Identifier: MIT
*/

package model

import (
	"encoding/json"
	"testing"
)

func TestExpressionDataJSONRoundTrip(t *testing.T) {
	t.Parallel()

	original := ExpressionData{
		Type: "bleAvatar",
		LeftEye: ExpressionItem{
			X:        1,
			Y:        2,
			Rotation: 3,
			Weight:   4,
			Size:     5,
		},
		RightEye: ExpressionItem{
			X:        6,
			Y:        7,
			Rotation: 8,
			Weight:   9,
			Size:     10,
		},
		Mouth: ExpressionItem{
			X:        11,
			Y:        12,
			Rotation: 13,
			Weight:   14,
			Size:     15,
		},
	}

	data, err := json.Marshal(original)
	if err != nil {
		t.Fatalf("marshal expression data: %v", err)
	}

	var decoded ExpressionData
	if err := json.Unmarshal(data, &decoded); err != nil {
		t.Fatalf("unmarshal expression data: %v", err)
	}

	if decoded != original {
		t.Fatalf("round-trip mismatch: got %+v want %+v", decoded, original)
	}
}

func TestMotionDataJSONRoundTrip(t *testing.T) {
	t.Parallel()

	original := MotionData{
		Type: "bleMotion",
		PitchServo: MotionDataItem{
			Angle: 450,
			Speed: 500,
		},
		YawServo: MotionDataItem{
			Rotate: 120,
			Speed:  300,
		},
	}

	data, err := json.Marshal(original)
	if err != nil {
		t.Fatalf("marshal motion data: %v", err)
	}

	var decoded MotionData
	if err := json.Unmarshal(data, &decoded); err != nil {
		t.Fatalf("unmarshal motion data: %v", err)
	}

	if decoded != original {
		t.Fatalf("round-trip mismatch: got %+v want %+v", decoded, original)
	}
}

func TestDanceDataJSONRoundTrip(t *testing.T) {
	t.Parallel()

	original := DanceData{
		LeftEye: ExpressionItem{Weight: 100},
		RightEye: ExpressionItem{
			Weight: 100,
		},
		Mouth:      ExpressionItem{Size: 25},
		PitchServo: MotionDataItem{Angle: 450, Speed: 900},
		YawServo:   MotionDataItem{Angle: 100, Speed: 700},
		DurationMs: 650,
	}

	data, err := json.Marshal(original)
	if err != nil {
		t.Fatalf("marshal dance data: %v", err)
	}

	var decoded DanceData
	if err := json.Unmarshal(data, &decoded); err != nil {
		t.Fatalf("unmarshal dance data: %v", err)
	}

	if decoded != original {
		t.Fatalf("round-trip mismatch: got %+v want %+v", decoded, original)
	}
}
