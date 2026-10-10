#include "window.hpp"
#include "cycle.hpp"
#include "glyph.hpp"

static bool needs_centering = false;
static bool hovered = false;
static bool moving = false;
static std::uint32_t cursor_offx = 0, cursor_offy = 0;

Window::Window(long id, std::string title, std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, std::uint8_t flags) : id(id), title(title), x(x), y(y), w(w), h(h)
{
    centered = flags & WINDOW_CENTERED;
    needs_centering = centered;
}

Window::~Window()
{
}

void Window::close()
{
    Cycle::windows.erase(id);
}

void Window::update(Framebuf &buf, const UI::Element &cursor)
{
    if (needs_centering)
    {
        std::uint32_t buf_w, buf_h;
        buf.size_get(&buf_w, &buf_h);
        x = buf_w / 2 - w / 2, y = buf_h / 2 - h / 2;
        needs_centering = false;
    }

    hovered = cursor.x >= x && cursor.x <= x + w &&
              cursor.y >= y && cursor.y <= y + WINDOW_TITLE_H;

    if (!moving && Cycle::mouse_l && hovered)
    {
        moving = true;
        cursor_offx = x - cursor.x, cursor_offy = y - cursor.y;
    }

    if (moving)
        if (Cycle::mouse_l)
            x = cursor.x + cursor_offx, y = cursor.y + cursor_offy;
        else
            moving = false;
}

void Window::render(Framebuf &buf)
{
    UI::Element title_pane = UI::rect(x, y, w, WINDOW_TITLE_H, moving ? WINDOW_PANE_COLOR : WINDOW_TITLE_PANE_COLOR);
    buf.px_render(title_pane);

    UI::Element title_text = UI::text(x + 4, y + WINDOW_TITLE_H / 2 - FONTC_HEIGHT / 2, (char *)title.c_str(), WINDOW_TITLE_COLOR);
    buf.px_render(title_text);

    UI::Element close_button = UI::button(x + w - 4, y, FONTC_WIDTH + 4, FONTC_HEIGHT + 4, "X");
    close_button.x -= close_button.button.w;
    close_button.y += WINDOW_TITLE_H / 2 - close_button.button.h / 2;
    buf.el_update(close_button);
    buf.px_render(close_button);
    if (close_button.button.active)
        close();

    UI::Element pane = UI::rect(x, y + WINDOW_TITLE_H, w, h, WINDOW_PANE_COLOR);
    buf.px_render(pane);
}
