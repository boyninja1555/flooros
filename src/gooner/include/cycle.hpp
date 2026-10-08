#pragma once

#include <linux/input.h>
#include <vector>
#include "fb.hpp"

namespace Cycle
{
    extern bool running;

    bool init(std::vector<UI::Element> &elements, Framebuf &buf);

    void cleanup();

    void update(std::vector<UI::Element> &elements, input_event *iev, int fd_input);

    void render(std::vector<UI::Element> &elements, Framebuf &buf);
}
