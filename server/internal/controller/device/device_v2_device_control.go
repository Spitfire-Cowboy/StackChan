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

func (c *ControllerV2) SendDeviceControl(ctx context.Context, req *v2.SendDeviceControlReq) (res *v2.SendDeviceControlRes, err error) {
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

	action, ok := buildDeviceControlAction(req.Action)
	if !ok {
		return nil, gerror.NewCode(gcode.CodeMissingParameter, "unsupported device control action")
	}

	if err := web_socket.SendDeviceControl(ctx, req.Mac, action); err != nil {
		return nil, gerror.WrapCode(gcode.CodeInternalError, err, "failed to send device control action")
	}

	return new(v2.SendDeviceControlRes(true)), nil
}
