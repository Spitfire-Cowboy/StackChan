/*
SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
SPDX-License-Identifier: MIT
*/

package model

import (
	"testing"
	"time"
)

func TestNewAppClientInitializesAndCloses(t *testing.T) {
	t.Parallel()

	client := NewAppClient("aa:bb:cc:dd:ee:ff", nil, "device-1")
	t.Cleanup(client.CloseWriterCoroutine)

	if got := client.GetMac(); got != "aa:bb:cc:dd:ee:ff" {
		t.Fatalf("unexpected mac: %q", got)
	}
	if got := client.GetDeviceId(); got != "device-1" {
		t.Fatalf("unexpected device id: %q", got)
	}
	if cap(client.SendChan()) != 100 {
		t.Fatalf("unexpected send channel capacity: %d", cap(client.SendChan()))
	}

	now := time.Unix(1700000000, 0)
	client.SetLastTime(now)
	if got := client.GetLastTime(); !got.Equal(now) {
		t.Fatalf("unexpected last time: %v", got)
	}

	client.CloseWriterCoroutine()
	waitForClosedChannel(t, client.SendChan())
}

func TestNewStackChanClientInitializesAndCloses(t *testing.T) {
	t.Parallel()

	client := NewStackChanClient("11:22:33:44:55:66", nil, nil, nil, true)
	t.Cleanup(client.CloseWriterCoroutine)

	if got := client.GetMac(); got != "11:22:33:44:55:66" {
		t.Fatalf("unexpected mac: %q", got)
	}
	if cap(client.SendChan()) != 100 {
		t.Fatalf("unexpected send channel capacity: %d", cap(client.SendChan()))
	}

	client.SetMac("66:55:44:33:22:11")
	if got := client.GetMac(); got != "66:55:44:33:22:11" {
		t.Fatalf("unexpected mac after set: %q", got)
	}

	client.CloseWriterCoroutine()
	waitForClosedChannel(t, client.SendChan())
}

func waitForClosedChannel(t *testing.T, ch chan *WsSendMsg) {
	t.Helper()

	deadline := time.After(2 * time.Second)
	for {
		select {
		case _, ok := <-ch:
			if !ok {
				return
			}
		case <-deadline:
			t.Fatal("timed out waiting for writer coroutine to close channel")
		default:
			time.Sleep(10 * time.Millisecond)
		}
	}
}
