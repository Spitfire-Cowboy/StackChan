/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <mooncake.h>
#include <memory>

namespace smooth_ui_toolkit::lvgl_cpp {
class Container;
class Label;
class Button;
}

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
    std::unique_ptr<smooth_ui_toolkit::lvgl_cpp::Container> _panel;
    std::unique_ptr<smooth_ui_toolkit::lvgl_cpp::Label> _title;
    std::unique_ptr<smooth_ui_toolkit::lvgl_cpp::Label> _subtitle;
    std::unique_ptr<smooth_ui_toolkit::lvgl_cpp::Label> _body;
    std::unique_ptr<smooth_ui_toolkit::lvgl_cpp::Button> _button_quit;
};

