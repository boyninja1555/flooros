#pragma once

#include <cstddef>
#include <cstdint>

#define UI_BUTTON_BACK 0xFFAFAFAF
#define UI_BUTTON_BACK_HOVER 0xFF9F9F9F
#define UI_BUTTON_BACK_ACTIVE 0xFFFF3D00
#define UI_BUTTON_FORE 0xFF080808

namespace UI
{
    typedef std::uint32_t color_t;

    namespace ElementData
    {
        struct Rect
        {
            std::uint32_t w, h;
            color_t color;
        };

        struct Image
        {
            uint32_t w, h;
            color_t *data;
        };

        struct Text
        {
            char *text;
            color_t color;
        };

        struct Button
        {
            uint32_t w, h;
            char *text;
            bool hover, active;
        };
    };

    struct Element
    {
        enum Type
        {
            RECT,
            IMAGE,
            TEXT,
            BUTTON,
        } type;
        std::int32_t x, y;
        union
        {
            ElementData::Rect rect;
            ElementData::Image image;
            ElementData::Text text;
            ElementData::Button button;
        };
    };

    Element rect(std::int32_t x, std::int32_t y, std::uint32_t w, std::uint32_t h, color_t color);

    Element image(std::int32_t x, std::int32_t y, std::uint32_t w, std::uint32_t h, color_t *data);

    Element text(std::int32_t x, std::int32_t y, char *text, color_t color);

    Element button(std::int32_t x, std::int32_t y, std::uint32_t w, std::uint32_t h, char *text);
};

class Framebuf
{
public:
    Framebuf();

    ~Framebuf();

    void init();

    void swap();

    void size_get(std::uint32_t *w, std::uint32_t *h);

    void el_update(UI::Element &element);

    void px_clear(UI::color_t color);

    void px_plot(std::uint32_t x, std::uint32_t y, UI::color_t color);

    void px_render(const UI::Element &element);

private:
    int fd;

    std::uint8_t *memback;
    std::uint8_t *memfront;
    std::size_t memsize;

    std::uint32_t width, height, pitch;
};
