#pragma once

#include <cstdint>
#include "fb.hpp"

namespace ImageLoad
{
    bool ppm(const char *filename, UI::color_t **data_ptr, uint32_t *w, uint32_t *h);
};
