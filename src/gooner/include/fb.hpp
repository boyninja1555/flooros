#pragma once

#include <cstddef>
#include <cstdint>

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
    };

    struct Element
    {
        enum Type
        {
            RECT,
            IMAGE,
            TEXT,
        } type;
        std::uint32_t x, y;
        union
        {
            ElementData::Text text;
            ElementData::Rect rect;
            ElementData::Image image;
        };
    };

    Element rect(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, color_t color);

    Element image(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, color_t *data);

    Element text(std::uint32_t x, std::uint32_t y, char *text, color_t color);
};

class Framebuf
{
public:
    Framebuf();

    ~Framebuf();

    void init();

    void swap();

    void size_get(std::uint32_t *w, std::uint32_t *h);

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
