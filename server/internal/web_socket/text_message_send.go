/*
SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
SPDX-License-Identifier: MIT
*/

package web_socket

import (
	"context"

	"stackChan/internal/wsproto"

	"github.com/gorilla/websocket"
)

func SendSpeechMessage(ctx context.Context, mac, name, content string) error {
	packet, err := wsproto.CreateTextMessagePacket(name, content)
	if err != nil {
		return err
	}

	messageType := websocket.BinaryMessage
	SendStackChanMessage(ctx, mac, &messageType, &packet, nil)
	return nil
}

func SendDisplayCard(ctx context.Context, mac, title string, lines []string, accent string, timeoutMs uint32, sticky bool, clear bool) error {
	packet, err := wsproto.CreateDisplayCardPacket(title, lines, accent, timeoutMs, sticky, clear)
	if err != nil {
		return err
	}

	messageType := websocket.BinaryMessage
	SendStackChanMessage(ctx, mac, &messageType, &packet, nil)
	return nil
}
