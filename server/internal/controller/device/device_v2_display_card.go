/*
SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
SPDX-License-Identifier: MIT
*/

package device

import (
	"context"
	"stackChan/internal/dao"
	"stackChan/internal/model"
	"stackChan/internal/web_socket"

	"github.com/gogf/gf/v2/errors/gcode"
	"github.com/gogf/gf/v2/errors/gerror"
	"github.com/gogf/gf/v2/frame/g"

	"stackChan/api/device/v2"
)

func (c *ControllerV2) SendDisplayCard(ctx context.Context, req *v2.SendDisplayCardReq) (res *v2.SendDisplayCardRes, err error) {
	uid := g.RequestFromCtx(ctx).GetCtxVar(model.Uid).Int64()
	if uid == 0 {
		return nil, gerror.NewCode(gcode.CodeMissingParameter, "user UID is required")
	}

	count, err := dao.Device.Ctx(ctx).
		Where("mac = ?", req.Mac).
		Where("uid = ?", uid).
		Count()
	if err != nil {
		return nil, gerror.WrapCode(gcode.CodeDbOperationError, err, "Failed to query device information")
	}
	if count == 0 {
		return nil, gerror.NewCode(gcode.CodeNotFound, "device not found or not belong to current user")
	}

	if !web_socket.IsStackChanOnline(req.Mac) {
		return nil, gerror.NewCode(gcode.CodeBusinessValidationFailed, "device is offline")
	}

	payload := buildDisplayCardPayload(req.Title, req.Lines, req.Accent, req.TimeoutMs)
	if len(payload.lines) == 0 {
		return nil, gerror.NewCode(gcode.CodeMissingParameter, "at least one non-empty display card line is required")
	}

	if err := web_socket.SendDisplayCard(ctx, req.Mac, payload.title, payload.lines, payload.accent, payload.timeoutMs); err != nil {
		return nil, gerror.WrapCode(gcode.CodeInternalError, err, "failed to send display card")
	}

	return new(v2.SendDisplayCardRes(true)), nil
}
