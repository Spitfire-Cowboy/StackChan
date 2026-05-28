/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include "ship_receipts_scene.h"
#include <mooncake.h>
#include <array>
#include <cstdint>
#include <deque>
#include <mutex>
#include <string>

/**
 * @brief Ship Receipts app shell.
 *
 * This is intentionally a thin app boundary on top of StackChan core:
 * generic device capabilities stay in core firmware, while Ship Receipts
 * mode/content can evolve as an app.
 */
class AppShipReceipts : public mooncake::AppAbility {
public:
    AppShipReceipts();

    void onCreate() override;
    void onOpen() override;
    void onRunning() override;
    void onClose() override;

private:
    enum class SequenceState {
        DemoRotation = 0,
        LiveOverride,
        LiveSticky,
    };

    static constexpr std::array<const char*, 4> _scene_json{{
        R"json({"id":"app-shell-card","mode":"ship-receipts-shell","title":"SHIP.RECEIPTS","line":"Host-driven scenes belong in the app layer.","presentation_type":"card","visual_template":"title_card","emotion":"neutral","duration_ms":3600,"motion":{"yaw_angle":0,"pitch_angle":0,"speed":420},"led":{"r":0,"g":0,"b":0},"play_notification":false})json",
        R"json({"id":"heike-avatar-1","mode":"heike","title":"HEIKE","speaker":"BIWA GUIDE","line":"Bell sounds. All things impermanent.","presentation_type":"avatar","visual_template":"avatar_beat","emotion":"sad","duration_ms":4200,"motion":{"yaw_angle":180,"pitch_angle":170,"speed":320},"led":{"r":64,"g":40,"b":0},"play_notification":true})json",
        R"json({"id":"odyssey-card-1","mode":"odyssey","title":"ODYSSEY","line":"A card beat can become status plus speech.","presentation_type":"card","visual_template":"status_card","emotion":"happy","duration_ms":3600,"motion":{"yaw_angle":-220,"pitch_angle":110,"speed":320},"led":{"r":0,"g":40,"b":72},"play_notification":false})json",
        R"json({"id":"app-boundary-avatar","mode":"ship-receipts-shell","title":"APP BOUNDARY","speaker":"STACKCHAN","line":"Motion, LEDs, and audio cues stay generic.","presentation_type":"avatar","visual_template":"avatar_beat","emotion":"doubtful","duration_ms":4200,"motion":{"yaw_angle":0,"pitch_angle":40,"speed":260},"led":{"r":36,"g":0,"b":56},"play_notification":true})json",
    }};

    bool loadScene(size_t index, ship_receipts::ScenePayload& out_scene);
    void enqueueSceneJson(std::string json);
    bool dequeueSceneJson(std::string& out_json);
    void handleQueuedCommand(const std::string& json);
    void applyBeat(const ship_receipts::ScenePayload& beat);
    void enterDemoRotation();
    void resumeDemoRotation();
    bool setDemoMode(std::string_view mode);
    const char* sequenceStateLabel() const;
    std::string statusSummary() const;
    void clearBeat();
    void advanceBeat();

    std::mutex _queue_mutex;
    std::deque<std::string> _pending_scene_json;
    uint32_t _beat_started_at = 0;
    size_t _beat_index        = 0;
    SequenceState _sequence_state = SequenceState::DemoRotation;
    ship_receipts::ScenePayload _active_scene{};
};
