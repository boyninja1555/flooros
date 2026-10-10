#include "fb.hpp"
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include "cycle.hpp"
#include "glyph.hpp"

UI::Element UI::rect(std::int32_t x, std::int32_t y, std::uint32_t w, std::uint32_t h, color_t color)
{
    UI::Element element = {.type = UI::Element::Type::RECT, .x = x, .y = y};
    element.rect.w = w;
    element.rect.h = h;
    element.rect.color = color;
    return element;
}

UI::Element UI::image(std::int32_t x, std::int32_t y, std::uint32_t w, std::uint32_t h, color_t *data)
{
    UI::Element element = {.type = UI::Element::Type::IMAGE, .x = x, .y = y};
    element.image.w = w;
    element.image.h = h;
    element.image.data = data;
    return element;
}

UI::Element UI::text(std::int32_t x, std::int32_t y, char *text, color_t color)
{
    UI::Element element = {.type = UI::Element::Type::TEXT, .x = x, .y = y};
    element.text.text = text;
    element.text.color = color;
    return element;
}

UI::Element UI::button(std::int32_t x, std::int32_t y, std::uint32_t w, std::uint32_t h, char *text)
{
    UI::Element element = {.type = UI::Element::Type::BUTTON, .x = x, .y = y};
    element.button.w = w;
    element.button.h = h;
    element.button.text = text;
    element.button.hover = false;
    element.button.active = false;
    return element;
}

Framebuf::Framebuf()
    : fd(-1),
      memback(nullptr),
      memfront(nullptr),
      memsize(0),
      width(0),
      height(0),
      pitch(0)
{
}

Framebuf::~Framebuf()
{
    if (fd != -1)
        close(fd);
    if (memback)
        std::free(memback);
    if (memfront)
        munmap(memfront, memsize);
}

void Framebuf::init()
{
    fd = open("/dev/fb0", O_RDWR);
    if (fd == -1)
        return;

    fb_fix_screeninfo fix{};
    fb_var_screeninfo var{};
    if (ioctl(fd, FBIOGET_FSCREENINFO, &fix) == -1)
        return;
    if (ioctl(fd, FBIOGET_VSCREENINFO, &var) == -1)
        return;
    memsize = fix.smem_len;

    memfront = static_cast<uint8_t *>(mmap(nullptr, memsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    if (memfront == MAP_FAILED)
    {
        memfront = nullptr;
        return;
    }

    memback = static_cast<uint8_t *>(std::malloc(memsize));
    if (!memback)
    {
        munmap(memfront, memsize);
        memfront = nullptr;
        return;
    }

    width = var.xres, height = var.yres, pitch = fix.line_length;
}

void Framebuf::swap()
{
    std::memcpy(memfront, memback, memsize);
}

void Framebuf::size_get(std::uint32_t *w, std::uint32_t *h)
{
    *w = width;
    *h = height;
}

void Framebuf::el_update(UI::Element &element)
{
    switch (element.type)
    {
    case UI::Element::Type::BUTTON:
    {
        element.button.active = (element.button.hover = Cycle::cursor.x >= element.x && Cycle::cursor.x <= element.x + element.button.w &&
                                                        Cycle::cursor.y >= element.y && Cycle::cursor.y <= element.y + element.button.h) &&
                                Cycle::mouse_l; // I know it's ugly but I... favor compact code? idfk FAJIFKHAWFNMFAWKAWF IT MUTATES LIKE 3 STATES gehehehHEeEHEHEHEHHEHEHHh
        break;
    }
    }
}

void Framebuf::px_clear(UI::color_t color)
{
    for (std::size_t i = 0; i < memsize; i += 4)
        std::memcpy(memback + i, &color, sizeof(std::uint32_t));
}

void Framebuf::px_plot(std::uint32_t x, std::uint32_t y, UI::color_t color)
{
    if (x < width && y < height)
    {
        std::size_t offset = y * pitch + x * 4;
        memback[offset] = color;
        memback[offset + 1] = color >> 8;
        memback[offset + 2] = color >> 16;
        memback[offset + 3] = color >> 24;
    }
}

void Framebuf::px_render(const UI::Element &element)
{
    switch (element.type)
    {
    case UI::Element::Type::RECT:
    {
        std::vector<std::uint32_t> row(element.rect.w, element.rect.color);
        for (std::uint32_t y = element.y; y < element.y + element.rect.h; y++)
            std::memcpy(memback + y * pitch + element.x * 4, row.data(), element.rect.w * 4);
        break;
    }

    case UI::Element::Type::IMAGE:
    {
        for (std::uint32_t row = 0; row < element.image.h; row++)
            std::memcpy(memback + (element.y + row) * pitch + element.x * 4, element.image.data + row * element.image.w, element.image.w * sizeof(UI::color_t));
        break;
    }

    case UI::Element::Type::TEXT:
    {
        std::size_t textlen = strlen(element.text.text);
        for (std::size_t i = 0; i < textlen; i++)
        {
            uint8_t glyph[FONTC_HEIGHT];
            Glyph::get(element.text.text[i], glyph);
            for (uint32_t row = 0; row < FONTC_HEIGHT; row++)
                for (uint32_t col = 0; col < FONTC_WIDTH; col++)
                    if (glyph[row] & (0x80 >> col))
                        px_plot(element.x + i * FONTC_WIDTH + col, element.y + row, element.text.color);
        }

        break;
    }

    case UI::Element::Type::BUTTON:
    {
        UI::Element back = UI::rect(element.x, element.y, element.button.w, element.button.h, element.button.active ? UI_BUTTON_BACK_ACTIVE : (element.button.hover ? UI_BUTTON_BACK_HOVER : UI_BUTTON_BACK));
        UI::Element fore = UI::text(element.x, element.y, element.button.text, UI_BUTTON_FORE);
        fore.x += back.rect.w / 2 - FONTC_WIDTH * strlen(fore.text.text) / 2;
        fore.y += back.rect.h / 2 - FONTC_WIDTH / 2;
        px_render(back);
        px_render(fore);
        break;
    }
    }
}
