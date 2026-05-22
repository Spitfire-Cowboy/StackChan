/*
SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
SPDX-License-Identifier: MIT
*/

package device

import "strings"

const (
	defaultDisplayCardTitle   = "STATUS"
	defaultDisplayCardAccent  = "#FFDF9A"
	defaultDisplayCardTimeout = 4000
	maxDisplayCardLines       = 6
)

type displayCardPayload struct {
	title     string
	lines     []string
	accent    string
	timeoutMs uint32
	sticky    bool
	clear     bool
}

func buildDisplayCardPayload(title string, lines []string, accent string, timeoutMs uint32, sticky bool, clear bool) displayCardPayload {
	normalizedTitle := strings.TrimSpace(title)
	if normalizedTitle == "" {
		normalizedTitle = defaultDisplayCardTitle
	}

	normalizedLines := make([]string, 0, maxDisplayCardLines)
	for _, line := range lines {
		line = strings.TrimSpace(line)
		if line == "" {
			continue
		}
		normalizedLines = append(normalizedLines, line)
		if len(normalizedLines) == maxDisplayCardLines {
			break
		}
	}

	normalizedAccent := strings.TrimSpace(accent)
	if normalizedAccent == "" {
		normalizedAccent = defaultDisplayCardAccent
	}

	if timeoutMs == 0 && !sticky && !clear {
		timeoutMs = defaultDisplayCardTimeout
	}

	return displayCardPayload{
		title:     normalizedTitle,
		lines:     normalizedLines,
		accent:    normalizedAccent,
		timeoutMs: timeoutMs,
		sticky:    sticky,
		clear:     clear,
	}
}
