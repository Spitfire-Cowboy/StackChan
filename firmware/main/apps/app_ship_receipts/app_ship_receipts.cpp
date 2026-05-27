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
#include <string_view>

using namespace mooncake;
using namespace stackchan;

namespace {

std::string buildSpeech(const ship_receipts::ScenePayload& beat)
{
    if (beat.speaker.empty()) {
        return beat.line;
    }

    if (beat.presentation_type == "card") {
        return fmt::format("{} · {}", beat.speaker, beat.line);
    }

    return fmt::format("{} says: {}", beat.speaker, beat.line);
}

const char* resolveEmotion(const ship_receipts::ScenePayload& beat)
{
    if (beat.presentation_type == "card") {
        return "neutral";
    }
    return beat.emotion.c_str();
}

}  // namespace

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

    std::unique_ptr<view::LoadingPage> loading_page;
    {
        LvglLockGuard lock;
        loading_page = std::make_unique<view::LoadingPage>(0x93C5FD, 0x0F172A);
        loading_page->setMessage("Starting\n scene services...");
    }

    GetHAL().startBleServer();
    GetHAL().startWebSocketAvatarService([&](std::string_view msg) {
        LvglLockGuard lock;
        loading_page->setMessage(msg);
    });

    {
        LvglLockGuard lock;
        loading_page.reset();

        auto avatar = std::make_unique<avatar::DefaultAvatar>();
        avatar->init(lv_screen_active());
        GetStackChan().attachAvatar(std::move(avatar));

        view::create_home_indicator([&]() { close(); }, 0x93C5FD, 0x0F172A);
        view::create_status_bar(0x93C5FD, 0x0F172A);
    }

    GetHAL().onBleConfigData.connect([&](const char* data) { enqueueSceneJson(data ? data : ""); });
    GetHAL().onWsTextMessage.connect([&](const WsTextMessage_t& message) {
        if (message.name == "ship-receipts") {
            enqueueSceneJson(message.content);
        }
    });

    _beat_index = 0;
    if (loadScene(_beat_index, _active_scene)) {
        applyBeat(_active_scene);
    }
    view::pop_a_toast("Ship Receipts demo shell ready", view::ToastType::Info, 1800);
}

void AppShipReceipts::onRunning()
{
    LvglLockGuard lock;

    std::string queued_scene;
    if (dequeueSceneJson(queued_scene)) {
        applyQueuedSceneJson(queued_scene);
    }

    GetStackChan().update();
    view::update_home_indicator();
    view::update_status_bar();

    if (_beat_started_at > 0) {
        if (GetHAL().millis() - _beat_started_at >= _active_scene.duration_ms) {
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
    GetHAL().onBleConfigData.clear();
    GetHAL().onWsTextMessage.clear();
    view::destroy_home_indicator();
    view::destroy_status_bar();

    GetHAL().requestWarmReboot(1);
}

bool AppShipReceipts::loadScene(size_t index, ship_receipts::ScenePayload& out_scene)
{
    std::string error_message;
    if (!ship_receipts::parse_scene_payload(_scene_json[index], out_scene, &error_message)) {
        mclog::tagError(getAppInfo().name, "failed to parse scene %u: %s", static_cast<unsigned>(index),
                        error_message.c_str());
        return false;
    }
    return true;
}

void AppShipReceipts::applyBeat(const ship_receipts::ScenePayload& beat)
{
    auto* display = Board::GetInstance().GetDisplay();
    auto& stack   = GetStackChan();
    auto& motion  = stack.motion();
    auto speech   = buildSpeech(beat);

    if (display) {
        display->SetStatus(beat.title.c_str());
        display->SetEmotion(resolveEmotion(beat));
        display->SetChatMessage("assistant", speech.c_str());
        display->ShowNotification(beat.title.c_str(), 1400);
    }

    if (beat.play_notification) {
        hal_bridge::app_play_sound(OGG_NEW_NOTIFICATION);
    }

    GetHAL().showRgbColor(beat.led_r, beat.led_g, beat.led_b);
    motion.moveWithSpeed(beat.yaw_angle, beat.pitch_angle, beat.speed);
    _beat_started_at = GetHAL().millis();
}

void AppShipReceipts::enqueueSceneJson(std::string json)
{
    std::lock_guard<std::mutex> lock(_queue_mutex);
    _pending_scene_json.push_back(std::move(json));
}

bool AppShipReceipts::dequeueSceneJson(std::string& out_json)
{
    std::lock_guard<std::mutex> lock(_queue_mutex);
    if (_pending_scene_json.empty()) {
        return false;
    }
    out_json = std::move(_pending_scene_json.front());
    _pending_scene_json.pop_front();
    return true;
}

void AppShipReceipts::applyQueuedSceneJson(const std::string& json)
{
    ship_receipts::ScenePayload parsed;
    std::string error_message;
    if (!ship_receipts::parse_scene_command(json.c_str(), parsed, &error_message)) {
        mclog::tagError(getAppInfo().name, "failed to parse incoming scene: %s", error_message.c_str());
        view::pop_a_toast("Ship Receipts scene parse failed", view::ToastType::Error, 2200);
        return;
    }

    _active_scene = std::move(parsed);
    applyBeat(_active_scene);
    view::pop_a_toast("Ship Receipts scene received", view::ToastType::Info, 1200);
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
    std::lock_guard<std::mutex> lock(_queue_mutex);
    _pending_scene_json.clear();
}

void AppShipReceipts::advanceBeat()
{
    _beat_index = (_beat_index + 1) % _scene_json.size();
    if (loadScene(_beat_index, _active_scene)) {
        applyBeat(_active_scene);
    }
}
