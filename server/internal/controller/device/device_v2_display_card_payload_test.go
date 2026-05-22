/*
SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
SPDX-License-Identifier: MIT
*/

package device

import "testing"

func TestBuildDisplayCardPayloadDefaults(t *testing.T) {
	payload := buildDisplayCardPayload("", nil, "", 0)

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
}

func TestBuildDisplayCardPayloadNormalizesLines(t *testing.T) {
	payload := buildDisplayCardPayload(
		"  SHIP RECEIPTS  ",
		[]string{"  Pro777  ", "", " Score 57 ", " ", "Streak 3d"},
		"  #123456  ",
		9000,
	)

	if payload.title != "SHIP RECEIPTS" {
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
	if payload.lines[0] != "Pro777" || payload.lines[1] != "Score 57" || payload.lines[2] != "Streak 3d" {
		t.Fatalf("unexpected lines: %#v", payload.lines)
	}
}

func TestBuildDisplayCardPayloadCapsLines(t *testing.T) {
	payload := buildDisplayCardPayload(
		"STATUS",
		[]string{"1", "2", "3", "4", "5", "6", "7"},
		"#FFDF9A",
		4000,
	)

	if len(payload.lines) != maxDisplayCardLines {
		t.Fatalf("expected %d lines, got %d", maxDisplayCardLines, len(payload.lines))
	}
	if payload.lines[maxDisplayCardLines-1] != "6" {
		t.Fatalf("expected last retained line to be 6, got %#v", payload.lines)
	}
}
