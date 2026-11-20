/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
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
#include <algorithm>
#include <atomic>
#include <cmath>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string_view>
#include <tuple>

using namespace mooncake;
using namespace stackchan;

namespace {

enum class WebsocketStartupState {
    Idle = 0,
    ConnectingNetwork,
    ReadyToRegister,
    Registered,
};

std::atomic<WebsocketStartupState> websocket_startup_state{WebsocketStartupState::Idle};

int clampValue(int value, int min_value, int max_value)
{
    return std::max(min_value, std::min(value, max_value));
}

uint8_t scaleColor(uint8_t value, float scale)
{
    return static_cast<uint8_t>(std::round(static_cast<float>(value) * scale));
}

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

std::string buildStatusLabel(const ship_receipts::ScenePayload& beat)
{
    if (beat.mode.empty()) {
        return "SHIP.RECEIPTS";
    }
    return fmt::format("SHIP.RECEIPTS · {}", beat.mode);
}

std::string buildAppReadyMessage(std::string_view mode)
{
    if (mode.empty()) {
        return "Ship Receipts app ready";
    }
    return fmt::format("Ship Receipts · {} mode", mode);
}

const char* resolveEmotion(const ship_receipts::ScenePayload& beat)
{
    if (beat.presentation_type == "card") {
        return "neutral";
    }
    return beat.emotion.c_str();
}

int resolveYawAngle(const ship_receipts::ScenePayload& beat)
{
    if (beat.presentation_type == "card") {
        return clampValue(beat.yaw_angle / 3, -60, 60);
    }
    return clampValue(beat.yaw_angle, -180, 180);
}

int resolvePitchAngle(const ship_receipts::ScenePayload& beat)
{
    if (beat.presentation_type == "card") {
        return clampValue(std::max(60, beat.pitch_angle / 2), 60, 140);
    }
    return clampValue(std::max(90, beat.pitch_angle), 90, 190);
}

int resolveMotionSpeed(const ship_receipts::ScenePayload& beat)
{
    if (beat.presentation_type == "card") {
        return clampValue(beat.speed / 2, 120, 220);
    }
    return clampValue(beat.speed, 180, 320);
}

std::tuple<uint8_t, uint8_t, uint8_t> resolveLedColor(const ship_receipts::ScenePayload& beat)
{
    const auto scale = beat.presentation_type == "card" ? 0.28f : 0.45f;
    return {
        scaleColor(beat.led_r, scale),
        scaleColor(beat.led_g, scale),
        scaleColor(beat.led_b, scale),
    };
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

    {
        LvglLockGuard lock;
        loading_page.reset();

        if (auto* display = Board::GetInstance().GetDisplay()) {
            display->SetupUI();
        }

        view::create_home_indicator([&]() { close(); }, 0x93C5FD, 0x0F172A);
        view::create_status_bar(0x93C5FD, 0x0F172A);

        _beat_label = lv_label_create(lv_screen_active());
        lv_obj_set_width(_beat_label, 300);
        lv_obj_align(_beat_label, LV_ALIGN_TOP_MID, 0, 8);
        lv_obj_set_style_text_align(_beat_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(_beat_label, lv_color_hex(0x0F172A), 0);
        lv_label_set_text(_beat_label, "SHIP.RECEIPTS");
    }

    GetHAL().onBleConfigData.connect([&](const char* data) { enqueueSceneJson(data ? data : ""); });
    GetHAL().onWsTextMessage.connect([&](const WsTextMessage_t& message) {
        if (message.name == "ship-receipts") {
            enqueueSceneJson(message.content);
        }
    });

    enterDemoRotation();
    view::pop_a_toast(buildAppReadyMessage(_selected_demo_mode), view::ToastType::Info, 1800);

    auto expected_state = WebsocketStartupState::Idle;
    if (websocket_startup_state.compare_exchange_strong(expected_state, WebsocketStartupState::ConnectingNetwork)) {
        auto task_created = xTaskCreate(
            [](void*) {
                GetHAL().startNetwork([](std::string_view) {});
                websocket_startup_state = WebsocketStartupState::ReadyToRegister;
                vTaskDelete(nullptr);
            },
            "ship_receipts_net", 4096, nullptr, 3, nullptr);
        if (task_created != pdPASS) {
            websocket_startup_state = WebsocketStartupState::Idle;
            mclog::tagError(getAppInfo().name, "failed to create network startup task");
        }
    }
}

void AppShipReceipts::onRunning()
{
    auto expected_state = WebsocketStartupState::ReadyToRegister;
    if (websocket_startup_state.compare_exchange_strong(expected_state, WebsocketStartupState::Registered)) {
        // onRunning is invoked by GetMooncake().update(), so ability registration
        // stays serialized with Mooncake's otherwise non-thread-safe update loop.
        GetHAL().startWebSocketAvatarService([](std::string_view) {});
    }

    LvglLockGuard lock;

    std::string queued_scene;
    if (dequeueSceneJson(queued_scene)) {
        handleQueuedCommand(queued_scene);
    }

    if (_beat_label) {
        auto label = buildStatusLabel(_active_scene);
        lv_label_set_text(_beat_label, label.c_str());
    }

    GetStackChan().update();
    view::update_home_indicator();
    view::update_status_bar();

    if (_beat_started_at > 0) {
        if (GetHAL().millis() - _beat_started_at >= _active_scene.duration_ms) {
            if (_sequence_state == SequenceState::LiveSticky) {
                _beat_started_at = 0;
            } else if (_sequence_state == SequenceState::LiveOverride) {
                resumeDemoRotation();
            } else {
                advanceBeat();
            }
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
    if (_beat_label) {
        lv_obj_delete(_beat_label);
        _beat_label = nullptr;
    }

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
    mclog::tagInfo(getAppInfo().name, "loaded scene id='{}' mode='{}' presentation='{}'", out_scene.scene_id,
                   out_scene.mode, out_scene.presentation_type);
    return true;
}

void AppShipReceipts::applyBeat(const ship_receipts::ScenePayload& beat)
{
    auto* display = Board::GetInstance().GetDisplay();
    auto& stack   = GetStackChan();
    auto& motion  = stack.motion();
    auto speech   = buildSpeech(beat);

    if (display) {
        display->SetEmotion(resolveEmotion(beat));
        display->SetChatMessage("assistant", speech.c_str());
    }

    mclog::tagInfo(getAppInfo().name, "apply beat id='{}' mode='{}' template='{}' sticky={} status='{}'", beat.scene_id,
                   beat.mode, beat.visual_template, beat.sticky, buildStatusLabel(beat));

    if (beat.play_notification) {
        mclog::tagInfo(getAppInfo().name, "skip notification audio for ship receipts beat '{}'", beat.scene_id);
    }

    const auto [led_r, led_g, led_b] = resolveLedColor(beat);
    const auto yaw_angle = resolveYawAngle(beat);
    const auto pitch_angle = resolvePitchAngle(beat);
    const auto motion_speed = resolveMotionSpeed(beat);

    mclog::tagInfo(getAppInfo().name,
                   "resolved beat id='{}' motion=({}, {}, {}) led=({}, {}, {})",
                   beat.scene_id, yaw_angle, pitch_angle, motion_speed,
                   static_cast<int>(led_r), static_cast<int>(led_g), static_cast<int>(led_b));

    GetHAL().showRgbColor(led_r, led_g, led_b);
    motion.moveWithSpeed(yaw_angle, pitch_angle, motion_speed);
    _beat_started_at = GetHAL().millis();
}

void AppShipReceipts::enterDemoRotation()
{
    _sequence_state = SequenceState::DemoRotation;
    if (!setDemoMode(_selected_demo_mode)) {
        _beat_index = 0;
        if (loadScene(_beat_index, _active_scene)) {
            applyBeat(_active_scene);
        }
        mclog::tagInfo(getAppInfo().name, "sequence state -> demo rotation (fallback)");
        return;
    }
    mclog::tagInfo(getAppInfo().name, "sequence state -> demo rotation");
}

void AppShipReceipts::resumeDemoRotation()
{
    _sequence_state = SequenceState::DemoRotation;
    const auto start_index = _beat_index;
    for (size_t offset = 1; offset <= _scene_json.size(); ++offset) {
        const auto next_index = (start_index + offset) % _scene_json.size();
        ship_receipts::ScenePayload candidate;
        if (!loadScene(next_index, candidate)) {
            continue;
        }
        if (!_selected_demo_mode.empty() && candidate.mode != _selected_demo_mode) {
            continue;
        }
        _beat_index   = next_index;
        _active_scene = std::move(candidate);
        applyBeat(_active_scene);
        mclog::tagInfo(getAppInfo().name, "sequence state -> demo rotation (resumed)");
        return;
    }
    if (loadScene(start_index, _active_scene)) {
        _beat_index = start_index;
        applyBeat(_active_scene);
    }
    mclog::tagInfo(getAppInfo().name, "sequence state -> demo rotation (single-mode fallback)");
}

bool AppShipReceipts::setDemoMode(std::string_view mode)
{
    for (size_t i = 0; i < _scene_json.size(); ++i) {
        ship_receipts::ScenePayload candidate;
        if (!loadScene(i, candidate)) {
            continue;
        }
        if (candidate.mode == mode) {
            _sequence_state = SequenceState::DemoRotation;
            _selected_demo_mode = std::string(mode);
            _beat_index     = i;
            _active_scene   = std::move(candidate);
            applyBeat(_active_scene);
            mclog::tagInfo(getAppInfo().name, "sequence state -> demo rotation (mode={})", mode);
            return true;
        }
    }
    return false;
}

const char* AppShipReceipts::sequenceStateLabel() const
{
    switch (_sequence_state) {
        case SequenceState::DemoRotation:
            return "demo";
        case SequenceState::LiveOverride:
            return "live";
        case SequenceState::LiveSticky:
            return "sticky";
    }
    return "unknown";
}

std::string AppShipReceipts::statusSummary() const
{
    return fmt::format("state={} mode={} selected={} scene={}", sequenceStateLabel(),
                       _active_scene.mode.empty() ? "-" : _active_scene.mode,
                       _selected_demo_mode.empty() ? "-" : _selected_demo_mode,
                       _active_scene.scene_id.empty() ? "-" : _active_scene.scene_id);
}

void AppShipReceipts::enqueueSceneJson(std::string json)
{
    if (json.size() > kMaxPendingSceneBytes) {
        mclog::tagWarn(getAppInfo().name, "drop oversized ship receipts message ({} bytes)",
                       static_cast<unsigned>(json.size()));
        return;
    }

    ship_receipts::ControlAction action;
    const bool is_control = ship_receipts::parse_control_command(json.c_str(), action);

    std::lock_guard<std::mutex> lock(_queue_mutex);
    while (_pending_scene_json.size() >= kMaxPendingSceneMessages ||
           _pending_scene_bytes + json.size() > kMaxPendingSceneBytes) {
        auto stale_scene = std::find_if(_pending_scene_json.begin(), _pending_scene_json.end(),
                                        [](const QueuedMessage& message) { return !message.is_control; });
        if (stale_scene != _pending_scene_json.end()) {
            _pending_scene_bytes -= stale_scene->json.size();
            _pending_scene_json.erase(stale_scene);
            continue;
        }
        if (!is_control) {
            mclog::tagWarn(getAppInfo().name, "drop ship receipts scene while control queue is full");
            return;
        }
        _pending_scene_bytes -= _pending_scene_json.front().json.size();
        _pending_scene_json.pop_front();
    }
    _pending_scene_bytes += json.size();
    _pending_scene_json.push_back({std::move(json), is_control});
}

bool AppShipReceipts::dequeueSceneJson(std::string& out_json)
{
    std::lock_guard<std::mutex> lock(_queue_mutex);
    if (_pending_scene_json.empty()) {
        return false;
    }
    _pending_scene_bytes -= _pending_scene_json.front().json.size();
    out_json = std::move(_pending_scene_json.front().json);
    _pending_scene_json.pop_front();
    return true;
}

void AppShipReceipts::handleQueuedCommand(const std::string& json)
{
    ship_receipts::ControlAction action;
    std::string mode;
    std::string control_error;
    if (ship_receipts::parse_control_command(json.c_str(), action, &mode, &control_error)) {
        if (action == ship_receipts::ControlAction::ResumeDemo) {
            resumeDemoRotation();
            view::pop_a_toast("Ship Receipts demo rotation resumed", view::ToastType::Info, 1200);
            return;
        }
        if (action == ship_receipts::ControlAction::ShowStatus) {
            auto summary = statusSummary();
            mclog::tagInfo(getAppInfo().name, "status -> {}", summary);
            view::pop_a_toast(summary, view::ToastType::Info, 1800);
            return;
        }
        if (action == ship_receipts::ControlAction::SetMode) {
            if (setDemoMode(mode)) {
                view::pop_a_toast(fmt::format("Ship Receipts mode -> {}", mode), view::ToastType::Info, 1500);
            } else {
                view::pop_a_toast(fmt::format("Unknown Ship Receipts mode: {}", mode), view::ToastType::Warning,
                                  1800);
            }
            return;
        }
    }

    ship_receipts::ScenePayload parsed;
    std::string error_message;
    if (!ship_receipts::parse_scene_command(json.c_str(), parsed, &error_message)) {
        mclog::tagError(getAppInfo().name, "failed to parse incoming scene: %s", error_message.c_str());
        view::pop_a_toast("Ship Receipts scene parse failed", view::ToastType::Error, 2200);
        return;
    }

    _sequence_state = parsed.sticky ? SequenceState::LiveSticky : SequenceState::LiveOverride;
    _active_scene = std::move(parsed);
    applyBeat(_active_scene);
    mclog::tagInfo(getAppInfo().name, "sequence state -> {}", _sequence_state == SequenceState::LiveSticky
                                                              ? "live sticky"
                                                              : "live override");
    view::pop_a_toast(_sequence_state == SequenceState::LiveSticky ? "Sticky Ship Receipts scene received"
                                                                   : "Ship Receipts scene received",
                      view::ToastType::Info, 1200);
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
    _pending_scene_bytes = 0;
}

void AppShipReceipts::advanceBeat()
{
    resumeDemoRotation();
}
