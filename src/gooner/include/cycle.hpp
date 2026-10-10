#pragma once

#include <linux/input.h>
#include <unordered_map>
#include <vector>
#include "fb.hpp"
#include "window.hpp"

namespace Cycle
{
    extern bool running;
    extern bool mouse_l, mouse_m, mouse_r;
    extern UI::Element cursor;
    extern std::unordered_map<long, Window> windows;

    bool init(std::vector<UI::Element> &elements, Framebuf &buf);

    void cleanup();

    void update(std::vector<UI::Element> &elements, Framebuf &buf, input_event *iev, int fd_keyboard, int fd_mouse);

    void render(std::vector<UI::Element> &elements, Framebuf &buf);
}
