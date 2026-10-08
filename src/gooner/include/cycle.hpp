#pragma once

#include <linux/input.h>
#include <vector>
#include "fb.hpp"

namespace Cycle
{
    extern bool running;

    bool init(std::vector<UI::Element> &elements, Framebuf &buf);

    void cleanup();

    void update(std::vector<UI::Element> &elements, Framebuf &buf, input_event *iev, int fd_keyboard, int fd_mouse);

    void render(std::vector<UI::Element> &elements, Framebuf &buf);
}
