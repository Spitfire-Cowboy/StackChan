/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "app_launcher.h"
#include <hal/hal.h>
#include <mooncake.h>
#include <mooncake_log.h>
#include <stackchan/stackchan.h>
#include <cstdint>

using namespace mooncake;

void AppLauncher::onLauncherCreate()
{
    mclog::tagInfo(getAppInfo().name, "on create");

    // 打开自己
    open();
}

void AppLauncher::onLauncherOpen()
{
    mclog::tagInfo(getAppInfo().name, "on open");

    LvglLockGuard lock;

    // For this fork, prefer entering SHIP.RECEIPTS directly instead of blocking
    // on first-run setup flows like Wi-Fi scanning.
    _startup_checked = true;
    _startup_worker.reset();
    _skip_initial_ship_receipts_open = GetHAL().getWarmRebootTarget() >= 0;
    create_launcher_view();
}

void AppLauncher::onLauncherRunning()
{
    LvglLockGuard lock;

    if (_startup_worker) {
        _startup_worker->update();
        if (_startup_worker->isDone()) {
            _startup_worker.reset();
            _startup_checked = true;
            create_launcher_view();
        }
    } else {
        _view->update();
        screensaver_update();
    }

    GetStackChan().update();
}

void AppLauncher::onLauncherClose()
{
    mclog::tagInfo(getAppInfo().name, "on close");

    LvglLockGuard lock;

    _view.reset();
}

void AppLauncher::onLauncherDestroy()
{
    mclog::tagInfo(getAppInfo().name, "on close");
}

void AppLauncher::create_launcher_view()
{
    _view = std::make_unique<view::LauncherView>();
    auto app_props = getAppProps();
    _view->init(app_props);
    _view->onAppClicked = [&](int appID) {
        mclog::tagInfo(getAppInfo().name, "handle open app, app id: {}", appID);
        openApp(appID);
    };

    maybe_open_initial_ship_receipts();
}

bool AppLauncher::maybe_open_initial_ship_receipts()
{
    if (_skip_initial_ship_receipts_open) {
        _skip_initial_ship_receipts_open = false;
        return false;
    }

    if (_opened_initial_ship_receipts) {
        return false;
    }

    for (const auto& props : getAppProps()) {
        if (props.info.name == "SHIP.RECEIPTS") {
            if (openApp(props.appID)) {
                _opened_initial_ship_receipts = true;
                mclog::tagInfo(getAppInfo().name, "auto-open ship receipts app, app id: {}", props.appID);
                return true;
            }
        }
    }

    return false;
}

void AppLauncher::screensaver_update()
{
    const uint32_t SCREENSAVER_TIMEOUT_MS = 30000;

    uint32_t idle_time = lv_display_get_inactive_time(NULL);
    if (idle_time >= SCREENSAVER_TIMEOUT_MS) {
        if (!_screensaver) {
            _screensaver = std::make_unique<view::Screensaver>();
            _screensaver->init();
        }
    } else if (_screensaver) {
        _screensaver.reset();
    }

    // Update in 30ms interval
    if (_screensaver && GetHAL().millis() - _screensaver_timecount > 30) {
        _screensaver_timecount = GetHAL().millis();
        _screensaver->update();
    }
}
