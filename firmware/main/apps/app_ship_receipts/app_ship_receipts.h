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

    static constexpr std::array<const char*, 6> _scene_json{{
        R"json({"id":"local-card-1","mode":"local","title":"LOCAL MODE","line":"Offline receipts, score loops, and streaks stay on-device.","presentation_type":"card","visual_template":"title_card","emotion":"neutral","duration_ms":3200,"motion":{"yaw_angle":0,"pitch_angle":0,"speed":420},"led":{"r":0,"g":48,"b":22},"play_notification":false})json",
        R"json({"id":"local-avatar-1","mode":"local","title":"LOCAL MODE","speaker":"BUILDER","line":"No accounts. No auth. Just build and score receipts.","presentation_type":"avatar","visual_template":"avatar_beat","emotion":"happy","duration_ms":3600,"motion":{"yaw_angle":140,"pitch_angle":160,"speed":320},"led":{"r":0,"g":64,"b":28},"play_notification":true})json",
        R"json({"id":"global-card-1","mode":"global","title":"GLOBAL MODE","line":"Proof envelopes can be exported for public verification.","presentation_type":"card","visual_template":"status_card","emotion":"neutral","duration_ms":3200,"motion":{"yaw_angle":-80,"pitch_angle":90,"speed":300},"led":{"r":0,"g":28,"b":72},"play_notification":false})json",
        R"json({"id":"global-avatar-1","mode":"global","title":"GLOBAL MODE","speaker":"LEDGER","line":"Verified receipts earn canonical reputation.","presentation_type":"avatar","visual_template":"avatar_beat","emotion":"doubtful","duration_ms":3800,"motion":{"yaw_angle":-220,"pitch_angle":110,"speed":320},"led":{"r":0,"g":40,"b":96},"play_notification":true})json",
        R"json({"id":"party-card-1","mode":"party","title":"PARTY MODE","line":"Track friends, rivals, and benchmarks beside your streaks.","presentation_type":"card","visual_template":"title_card","emotion":"neutral","duration_ms":3200,"motion":{"yaw_angle":80,"pitch_angle":40,"speed":300},"led":{"r":72,"g":24,"b":72},"play_notification":false})json",
        R"json({"id":"party-avatar-1","mode":"party","title":"PARTY MODE","speaker":"SCOUT","line":"Add GitHub users to the party and chase better proofs.","presentation_type":"avatar","visual_template":"avatar_beat","emotion":"happy","duration_ms":3800,"motion":{"yaw_angle":220,"pitch_angle":120,"speed":320},"led":{"r":96,"g":28,"b":96},"play_notification":true})json",
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
    std::string _selected_demo_mode = "local";
    ship_receipts::ScenePayload _active_scene{};
};
