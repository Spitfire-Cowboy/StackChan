/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "app_ship_receipts.h"
#include <apps/common/common.h>
#include <assets/assets.h>
#include <board.h>
#include <display.h>
#include <hal/hal.h>
#include <hal/board/hal_bridge.h>
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

        view::create_home_indicator([&]() { close(); }, 0x93C5FD, 0x0F172A);
        view::create_status_bar(0x93C5FD, 0x0F172A);
    }

    _beat_index = 0;
    applyBeat(_beats[_beat_index]);
    view::pop_a_toast("Ship Receipts demo shell ready", view::ToastType::Info, 1800);
}

void AppShipReceipts::onRunning()
{
    LvglLockGuard lock;

    GetStackChan().update();
    view::update_home_indicator();
    view::update_status_bar();

    if (_beat_started_at > 0) {
        const auto& beat = _beats[_beat_index];
        if (GetHAL().millis() - _beat_started_at >= beat.duration_ms) {
            advanceBeat();
        }
    }
}

void AppShipReceipts::onClose()
{
    mclog::tagInfo(getAppInfo().name, "on close");

    LvglLockGuard lock;

    clearBeat();
    GetStackChan().resetAvatar();
    view::destroy_home_indicator();
    view::destroy_status_bar();
}

void AppShipReceipts::applyBeat(const Beat& beat)
{
    auto* display = Board::GetInstance().GetDisplay();
    auto& stack   = GetStackChan();
    auto& motion  = stack.motion();

    if (display) {
        display->SetStatus(beat.title.data());
        display->SetEmotion(beat.emotion.data());
        display->SetChatMessage("assistant", beat.line.data());
        display->ShowNotification(beat.title.data(), 1400);
    }

    if (beat.play_notification) {
        hal_bridge::app_play_sound(OGG_NEW_NOTIFICATION);
    }

    GetHAL().showRgbColor(beat.led_r, beat.led_g, beat.led_b);
    motion.moveWithSpeed(beat.yaw_angle, beat.pitch_angle, beat.speed);
    _beat_started_at = GetHAL().millis();
}

void AppShipReceipts::clearBeat()
{
    if (auto* display = Board::GetInstance().GetDisplay()) {
        display->ClearChatMessages();
        display->SetStatus("");
    }

    GetHAL().showRgbColor(0, 0, 0);
    GetStackChan().motion().goHome(320);
    _beat_started_at = 0;
}

void AppShipReceipts::advanceBeat()
{
    _beat_index = (_beat_index + 1) % _beats.size();
    applyBeat(_beats[_beat_index]);
}
