/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "ws_display_card.h"
#include <algorithm>
#include <sstream>

namespace view {

static std::string join_lines(const std::vector<std::string>& lines)
{
    std::ostringstream builder;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (i > 0) {
            builder << '\n';
        }
        builder << lines[i];
    }
    return builder.str();
}

WsDisplayCardView::WsDisplayCardView(lv_obj_t* parent, std::string title, const std::vector<std::string>& lines,
                                     uint32_t durationMs, uint32_t accentHex)
{
    _durationMs = std::max<uint32_t>(durationMs, 1000);
    _startTick  = GetHAL().millis();

    _panel = std::make_unique<uitk::lvgl_cpp::Container>(parent ? parent : lv_screen_active());
    _panel->setBgColor(lv_color_hex(0x000000));
    _panel->setBgOpa(LV_OPA_50);
    _panel->removeFlag(LV_OBJ_FLAG_SCROLLABLE);
    _panel->align(LV_ALIGN_CENTER, 0, 0);
    _panel->setBorderWidth(0);
    _panel->setSize(320, 240);
    _panel->setRadius(0);
    _panel->onClick().connect([this]() { requestDestroy(); });

    _card = std::make_unique<uitk::lvgl_cpp::Container>(_panel->get());
    _card->setBgColor(lv_color_hex(accentHex));
    _card->setBorderColor(lv_color_hex(0x47330A));
    _card->setBorderWidth(2);
    _card->removeFlag(LV_OBJ_FLAG_SCROLLABLE);
    _card->align(LV_ALIGN_CENTER, 0, 0);
    _card->setSize(296, 164);
    _card->setRadius(18);
    _card->setPadding(18, 18, 16, 16);

    _title = std::make_unique<uitk::lvgl_cpp::Label>(_card->get());
    _title->align(LV_ALIGN_TOP_LEFT, 0, 0);
    _title->setWidth(260);
    _title->setText(title.empty() ? "STATUS" : title);
    _title->setTextFont(&lv_font_montserrat_20);
    _title->setTextColor(lv_color_hex(0x47330A));

    _content = std::make_unique<uitk::lvgl_cpp::Label>(_card->get());
    _content->align(LV_ALIGN_TOP_LEFT, 0, 38);
    _content->setWidth(260);
    _content->setText(join_lines(lines));
    _content->setTextFont(&lv_font_montserrat_24);
    _content->setTextColor(lv_color_hex(0x2C2110));
    _content->setTextAlign(LV_TEXT_ALIGN_LEFT);
    _content->setLongMode(LV_LABEL_LONG_WRAP);
}

WsDisplayCardView::~WsDisplayCardView()
{
    if (onDestroy) {
        onDestroy();
    }
}

void WsDisplayCardView::_update()
{
    if (GetHAL().millis() - _startTick >= _durationMs) {
        requestDestroy();
    }
}

}  // namespace view
