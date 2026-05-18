/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <lvgl.h>
#include <smooth_lvgl.hpp>
#include <uitk/short_namespace.hpp>
#include <stackchan/stackchan.h>
#include <hal/hal.h>
#include <memory>
#include <string>
#include <vector>

namespace view {

class WsDisplayCardView : public stackchan::avatar::Decorator {
public:
    std::function<void(void)> onDestroy;

    WsDisplayCardView(lv_obj_t* parent, std::string title, const std::vector<std::string>& lines, uint32_t durationMs,
                      uint32_t accentHex = 0xFFDF9A, bool sticky = false);

    ~WsDisplayCardView() override;

private:
    void _update() override;

    std::unique_ptr<uitk::lvgl_cpp::Container> _panel;
    std::unique_ptr<uitk::lvgl_cpp::Container> _card;
    std::unique_ptr<uitk::lvgl_cpp::Label> _title;
    std::unique_ptr<uitk::lvgl_cpp::Label> _content;

    uint32_t _startTick  = 0;
    uint32_t _durationMs = 4000;
    bool _sticky         = false;
};

}  // namespace view
