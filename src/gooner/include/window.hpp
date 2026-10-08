#pragma once

#include <cstdint>
#include <string>
#include "fb.hpp"

#define WINDOW_TITLE_H 24
#define WINDOW_PANE_COLOR 0xFFCFCFCF
#define WINDOW_TITLE_PANE_COLOR 0xFFAFAFAF
#define WINDOW_TITLE_COLOR 0xFF080808

#define WINDOW_CENTERED 0x01

class Window
{
public:
    Window(std::string title, std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, std::uint8_t flags);

    ~Window();

    void update(Framebuf &buf, const UI::Element &cursor);

    void render(Framebuf &buf);

private:
    std::string title;
    std::uint32_t x, y, w, h;
    bool centered;
};
