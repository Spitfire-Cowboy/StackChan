/*
SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
SPDX-License-Identifier: MIT
*/

package device

import "testing"

func TestBuildDisplayCardPayloadDefaults(t *testing.T) {
	payload := buildDisplayCardPayload("", nil, "", 0, false, false)

	if payload.title != defaultDisplayCardTitle {
		t.Fatalf("expected default title %q, got %q", defaultDisplayCardTitle, payload.title)
	}
	if payload.accent != defaultDisplayCardAccent {
		t.Fatalf("expected default accent %q, got %q", defaultDisplayCardAccent, payload.accent)
	}
	if payload.timeoutMs != defaultDisplayCardTimeout {
		t.Fatalf("expected default timeout %d, got %d", defaultDisplayCardTimeout, payload.timeoutMs)
	}
	if len(payload.lines) != 0 {
		t.Fatalf("expected no lines, got %#v", payload.lines)
	}
	if payload.sticky || payload.clear {
		t.Fatalf("expected default payload flags to be false, got %#v", payload)
	}
}

func TestBuildDisplayCardPayloadNormalizesLines(t *testing.T) {
	payload := buildDisplayCardPayload(
		"  SYSTEM NOTICE  ",
		[]string{"  Wi-Fi Connected  ", "", " Battery 82% ", " ", "Camera Ready"},
		"  #123456  ",
		9000,
		false,
		false,
	)

	if payload.title != "SYSTEM NOTICE" {
		t.Fatalf("expected trimmed title, got %q", payload.title)
	}
	if payload.accent != "#123456" {
		t.Fatalf("expected trimmed accent, got %q", payload.accent)
	}
	if payload.timeoutMs != 9000 {
		t.Fatalf("expected timeout 9000, got %d", payload.timeoutMs)
	}
	if len(payload.lines) != 3 {
		t.Fatalf("expected 3 lines, got %#v", payload.lines)
	}
	if payload.lines[0] != "Wi-Fi Connected" || payload.lines[1] != "Battery 82%" || payload.lines[2] != "Camera Ready" {
		t.Fatalf("unexpected lines: %#v", payload.lines)
	}
}

func TestBuildDisplayCardPayloadCapsLines(t *testing.T) {
	payload := buildDisplayCardPayload(
		"STATUS",
		[]string{"1", "2", "3", "4", "5", "6", "7"},
		"#FFDF9A",
		4000,
		false,
		false,
	)

	if len(payload.lines) != maxDisplayCardLines {
		t.Fatalf("expected %d lines, got %d", maxDisplayCardLines, len(payload.lines))
	}
	if payload.lines[maxDisplayCardLines-1] != "6" {
		t.Fatalf("expected last retained line to be 6, got %#v", payload.lines)
	}
}

func TestBuildDisplayCardPayloadStickyPreservesZeroTimeout(t *testing.T) {
	payload := buildDisplayCardPayload("STATUS", []string{"Tap to continue"}, "", 0, true, false)

	if !payload.sticky {
		t.Fatalf("expected sticky payload, got %#v", payload)
	}
	if payload.timeoutMs != 0 {
		t.Fatalf("expected sticky payload timeout to remain 0, got %d", payload.timeoutMs)
	}
}

func TestBuildDisplayCardPayloadClearAllowsEmptyLines(t *testing.T) {
	payload := buildDisplayCardPayload("", nil, "", 0, false, true)

	if !payload.clear {
		t.Fatalf("expected clear payload, got %#v", payload)
	}
	if len(payload.lines) != 0 {
		t.Fatalf("expected clear payload to allow empty lines, got %#v", payload.lines)
	}
	if payload.timeoutMs != 0 {
		t.Fatalf("expected clear payload timeout to remain 0, got %d", payload.timeoutMs)
	}
}
