/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <mooncake.h>
#include <cstdint>
#include <array>
#include <string_view>

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
    struct Beat {
        std::string_view title;
        std::string_view line;
        std::string_view emotion;
        uint32_t duration_ms;
        int yaw_angle;
        int pitch_angle;
        int speed;
        uint8_t led_r;
        uint8_t led_g;
        uint8_t led_b;
        bool play_notification;
    };

    static constexpr std::array<Beat, 4> _beats{{
        {"SHIP.RECEIPTS", "Host-driven scenes belong in the app layer.", "neutral", 3600, 0, 0, 420, 0, 0, 0,
         false},
        {"HEIKE", "Bell sounds. All things impermanent.", "sad", 4200, 180, 170, 320, 64, 40, 0, true},
        {"ODYSSEY", "A card beat can become status plus speech.", "happy", 3600, -220, 110, 320, 0, 40, 72,
         false},
        {"APP BOUNDARY", "Motion, LEDs, and audio cues stay generic.", "doubtful", 4200, 0, 40, 260, 36, 0, 56,
         true},
    }};

    void applyBeat(const Beat& beat);
    void clearBeat();
    void advanceBeat();

    uint32_t _beat_started_at = 0;
    size_t _beat_index        = 0;
};
