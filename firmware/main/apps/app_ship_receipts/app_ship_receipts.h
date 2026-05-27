/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <mooncake.h>
#include <cstdint>

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
    uint32_t _open_tick = 0;
};
