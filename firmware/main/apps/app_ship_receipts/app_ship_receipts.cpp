/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "app_ship_receipts.h"
#include <apps/common/common.h>
#include <assets/assets.h>
#include <hal/hal.h>
#include <mooncake_log.h>
#include <stackchan/stackchan.h>
#include <smooth_lvgl.hpp>

using namespace mooncake;
using namespace stackchan;

AppShipReceipts::AppShipReceipts()
{
    setAppInfo().name = "SHIP.RECEIPTS";

    static auto icon  = assets::get_image("icon_sentinel.bin");
    setAppInfo().icon = (void*)&icon;

    static uint32_t theme_color = 0x4A79E8;
    setAppInfo().userData       = (void*)&theme_color;
}

void AppShipReceipts::onCreate()
{
    mclog::tagInfo(getAppInfo().name, "on create");
}

void AppShipReceipts::onOpen()
{
    mclog::tagInfo(getAppInfo().name, "on open");

    {
        LvglLockGuard lock;

        auto avatar = std::make_unique<avatar::DefaultAvatar>();
        avatar->init(lv_screen_active());
        GetStackChan().attachAvatar(std::move(avatar));

        auto& stackchan = GetStackChan();
        stackchan.avatar().setSpeech(
            "Ship Receipts should live as an app. Keep motion, audio, and diagnostics in core.");
        stackchan.addModifier(std::make_unique<TimedSpeechModifier>(
            "Host-driven cards and avatar beats belong here.", 5000));

        view::create_home_indicator([&]() { close(); }, 0x93C5FD, 0x0F172A);
        view::create_status_bar(0x93C5FD, 0x0F172A);
        view::pop_a_toast("Ship Receipts app shell ready", view::ToastType::Info, 1800);
    }

    _open_tick = GetHAL().millis();
}

void AppShipReceipts::onRunning()
{
    LvglLockGuard lock;

    GetStackChan().update();
    view::update_home_indicator();
    view::update_status_bar();

    if (_open_tick > 0 && GetHAL().millis() - _open_tick > 6000) {
        GetStackChan().avatar().setSpeech("");
        _open_tick = 0;
    }
}

void AppShipReceipts::onClose()
{
    mclog::tagInfo(getAppInfo().name, "on close");

    LvglLockGuard lock;

    GetStackChan().resetAvatar();
    view::destroy_home_indicator();
    view::destroy_status_bar();
}

