/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <cstdint>
#include <string>

namespace ship_receipts {

struct ScenePayload {
    std::string title        = "SHIP.RECEIPTS";
    std::string line         = "";
    std::string emotion      = "neutral";
    uint32_t duration_ms     = 3600;
    int yaw_angle            = 0;
    int pitch_angle          = 0;
    int speed                = 320;
    uint8_t led_r            = 0;
    uint8_t led_g            = 0;
    uint8_t led_b            = 0;
    bool play_notification   = false;
};

bool parse_scene_payload(const char* json, ScenePayload& out_payload, std::string* error_message = nullptr);

}  // namespace ship_receipts
