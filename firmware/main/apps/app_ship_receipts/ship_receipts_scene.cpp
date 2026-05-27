/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "ship_receipts_scene.h"
#include <ArduinoJson.hpp>

namespace ship_receipts {

namespace {

void set_error(std::string* error_message, const char* message)
{
    if (error_message) {
        *error_message = message;
    }
}

uint8_t clamp_color(int value)
{
    if (value < 0) {
        return 0;
    }
    if (value > 255) {
        return 255;
    }
    return static_cast<uint8_t>(value);
}

}  // namespace

bool parse_scene_payload(const char* json, ScenePayload& out_payload, std::string* error_message)
{
    if (!json) {
        set_error(error_message, "scene payload was null");
        return false;
    }

    ArduinoJson::JsonDocument doc;
    auto error = ArduinoJson::deserializeJson(doc, json);
    if (error) {
        set_error(error_message, error.c_str());
        return false;
    }

    if (doc["title"].is<const char*>()) {
        out_payload.title = doc["title"].as<const char*>();
    }
    if (doc["line"].is<const char*>()) {
        out_payload.line = doc["line"].as<const char*>();
    }
    if (doc["emotion"].is<const char*>()) {
        out_payload.emotion = doc["emotion"].as<const char*>();
    }
    if (doc["duration_ms"].is<uint32_t>()) {
        out_payload.duration_ms = doc["duration_ms"].as<uint32_t>();
    }
    if (doc["play_notification"].is<bool>()) {
        out_payload.play_notification = doc["play_notification"].as<bool>();
    }

    if (doc["motion"].is<ArduinoJson::JsonObject>()) {
        auto motion = doc["motion"].as<ArduinoJson::JsonObject>();
        if (motion["yaw_angle"].is<int>()) {
            out_payload.yaw_angle = motion["yaw_angle"].as<int>();
        }
        if (motion["pitch_angle"].is<int>()) {
            out_payload.pitch_angle = motion["pitch_angle"].as<int>();
        }
        if (motion["speed"].is<int>()) {
            out_payload.speed = motion["speed"].as<int>();
        }
    }

    if (doc["led"].is<ArduinoJson::JsonObject>()) {
        auto led = doc["led"].as<ArduinoJson::JsonObject>();
        if (led["r"].is<int>()) {
            out_payload.led_r = clamp_color(led["r"].as<int>());
        }
        if (led["g"].is<int>()) {
            out_payload.led_g = clamp_color(led["g"].as<int>());
        }
        if (led["b"].is<int>()) {
            out_payload.led_b = clamp_color(led["b"].as<int>());
        }
    }

    return true;
}

bool parse_scene_command(const char* json, ScenePayload& out_payload, std::string* error_message)
{
    if (!json) {
        set_error(error_message, "scene command was null");
        return false;
    }

    ArduinoJson::JsonDocument doc;
    auto error = ArduinoJson::deserializeJson(doc, json);
    if (error) {
        set_error(error_message, error.c_str());
        return false;
    }

    if (doc["cmd"].is<const char*>() && std::string_view(doc["cmd"].as<const char*>()) == "shipReceiptsScene") {
        if (!doc["data"].is<ArduinoJson::JsonObject>()) {
            set_error(error_message, "shipReceiptsScene command missing object data");
            return false;
        }

        std::string nested;
        ArduinoJson::serializeJson(doc["data"], nested);
        return parse_scene_payload(nested.c_str(), out_payload, error_message);
    }

    return parse_scene_payload(json, out_payload, error_message);
}

}  // namespace ship_receipts
