#pragma once

#include <cstdint>

#define FONTC_WIDTH 8
#define FONTC_HEIGHT 8

namespace Glyph
{
    void get(char c, std::uint8_t glyph_ptr[FONTC_HEIGHT]);
};
