/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "app_ship_receipts.h"
#include <hal/hal.h>
#include <mooncake_log.h>
#include <smooth_lvgl.hpp>

using namespace mooncake;
using namespace smooth_ui_toolkit::lvgl_cpp;

AppShipReceipts::AppShipReceipts()
{
    setAppInfo().name = "SHIP.RECEIPTS";

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

    LvglLockGuard lock;

    _panel = std::make_unique<Container>(lv_screen_active());
    _panel->setSize(320, 240);
    _panel->setAlign(LV_ALIGN_CENTER);
    _panel->setBorderWidth(0);
    _panel->setRadius(0);
    _panel->setPaddingAll(0);
    _panel->setBgColor(lv_color_hex(0x0F172A));
    _panel->setBgOpa(LV_OPA_100);

    _title = std::make_unique<Label>(_panel->get());
    _title->setText("SHIP RECEIPTS");
    _title->setTextFont(&lv_font_montserrat_24);
    _title->setTextColor(lv_color_hex(0xF8FAFC));
    _title->align(LV_ALIGN_TOP_MID, 0, 20);

    _subtitle = std::make_unique<Label>(_panel->get());
    _subtitle->setText("App shell on top of StackChan core");
    _subtitle->setTextFont(&lv_font_montserrat_14);
    _subtitle->setTextColor(lv_color_hex(0x93C5FD));
    _subtitle->alignTo(_title->get(), LV_ALIGN_OUT_BOTTOM_MID, 0, 8);

    _body = std::make_unique<Label>(_panel->get());
    _body->setWidth(268);
    _body->setLongMode(LV_LABEL_LONG_WRAP);
    _body->setText(
        "This app boundary is where host-driven game modes belong.\n\n"
        "- authored scene packets\n"
        "- cards and avatar beats\n"
        "- ship-receipts copy and rules\n\n"
        "Keep transport, motion, audio, diagnostics, and render primitives in core.");
    _body->setTextFont(&lv_font_montserrat_14);
    _body->setTextColor(lv_color_hex(0xE2E8F0));
    _body->align(LV_ALIGN_TOP_MID, 0, 82);

    _button_quit = std::make_unique<Button>(_panel->get());
    _button_quit->setSize(140, 42);
    _button_quit->setBgColor(lv_color_hex(0x1D4ED8));
    _button_quit->align(LV_ALIGN_BOTTOM_MID, 0, -18);
    _button_quit->label().setText("BACK");
    _button_quit->label().setTextColor(lv_color_hex(0xFFFFFF));
    _button_quit->onClick().connect([this]() { close(); });
}

void AppShipReceipts::onRunning()
{
}

void AppShipReceipts::onClose()
{
    mclog::tagInfo(getAppInfo().name, "on close");

    LvglLockGuard lock;

    _button_quit.reset();
    _body.reset();
    _subtitle.reset();
    _title.reset();
    _panel.reset();
}

