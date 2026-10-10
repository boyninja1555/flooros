#include "cycle.hpp"
#include <unistd.h>
#include <cstring>
#include <unordered_map>
#include <iostream>
#include "glyph.hpp"
#include "window.hpp"
#include "image.hpp"

#define TASKBAR_H 32

static UI::color_t *logo24;
static UI::color_t *cursor_img;

static std::unordered_map<long, Window> windows;
static UI::Element quitbtn;

bool Cycle::running = true;
bool Cycle::mouse_l = false, Cycle::mouse_m = false, Cycle::mouse_r = false;
UI::Element Cycle::cursor;

static void window_close(long id)
{
    windows.erase(id);
}

bool Cycle::init(std::vector<UI::Element> &elements, Framebuf &buf)
{
    std::uint32_t buf_w, buf_h;
    buf.size_get(&buf_w, &buf_h);
    if (buf_w == 0 || buf_h == 0)
    {
        std::cerr << "Unable to launch gooner! Buffer initialization did not succeed, likely due to a missing graphics option on your system." << std::endl;
        return false;
    }

    uint32_t logo_w, logo_h;
    ImageLoad::ppm("/etc/logo24.ppm", &logo24, &logo_w, &logo_h);
    if (!logo24 || logo_w == 0 || logo_h == 0)
    {
        logo_w = 24, logo_h = 24;
        size_t logo_size = logo_w * logo_h * 4;
        logo24 = (UI::color_t *)std::malloc(logo_size);
        std::memset(logo24, 0, logo_size);
        std::cerr << "Logo image (/etc/logo24.ppm) failed to load! Using default image data." << std::endl;
    }

    std::uint32_t cursor_w, cursor_h;
    ImageLoad::ppm("/etc/cursor.ppm", &cursor_img, &cursor_w, &cursor_h);
    if (!cursor_img || cursor_w == 0 || cursor_h == 0)
    {
        cursor_w = 24, cursor_h = 24;
        size_t cursor_size = cursor_w * cursor_h * 4;
        cursor_img = (UI::color_t *)std::malloc(cursor_size);
        std::memset(cursor_img, 0, cursor_size);
        std::cerr << "Cursor image (/etc/cursor.ppm) failed to load! Using default image data." << std::endl;
    }

    std::uint32_t cursor_x = buf_w / 2, cursor_y = buf_h / 2;
    cursor = UI::image(cursor_x, cursor_y, cursor_w, cursor_h, cursor_img);

    UI::Element taskbar_rect = UI::rect(0, buf_h - TASKBAR_H, buf_w, TASKBAR_H, 0xFF0040FF);
    UI::Element taskbar_logo = UI::image(4, buf_h - TASKBAR_H + TASKBAR_H / 2 - logo_h / 2, logo_w, logo_h, logo24);
    UI::Element taskbar_name = UI::text(4 + logo_w + 4, buf_h - TASKBAR_H + TASKBAR_H / 2 - FONTC_HEIGHT / 2, "FLOOROS", 0xFFDEDEDE);
    elements.push_back(taskbar_rect);
    elements.push_back(taskbar_logo);
    elements.push_back(taskbar_name);

    quitbtn = UI::button(buf_w - 4, buf_h - TASKBAR_H + TASKBAR_H / 2 - (FONTC_HEIGHT * 2) / 2, 64, FONTC_HEIGHT * 2, "QUIT");
    quitbtn.x -= quitbtn.button.w;

    // Temporary
    UI::Element temp_text1 = UI::text(4, 4, "WELCOME TO THE FLOOROS GOONER DESKTOP!", 0xFFDEDEDE);
    UI::Element temp_text2 = UI::text(4, 8 + FONTC_HEIGHT, "AS OF NOW, THERE IS NOT MUCH TO SEE HERE.", 0xFFDEDEDE);
    UI::Element temp_text3 = UI::text(4, 12 + FONTC_HEIGHT * 2, "THE GITHUB HAS A TON OF COMMIT MESSAGES THAT MAY OR MAY NOT RELATE TO GOONER UPDATES.", 0xFFDEDEDE);
    UI::Element temp_text4 = UI::text(4, 16 + FONTC_HEIGHT * 3, "ENJOY QUITTING!", 0xFFDEDEDE);
    elements.push_back(temp_text1);
    elements.push_back(temp_text2);
    elements.push_back(temp_text3);
    elements.push_back(temp_text4);

    windows.try_emplace(0L, 0L, window_close, "WELCOME", 0, 0, 800, 500, WINDOW_CENTERED);
    return true;
}

void Cycle::cleanup()
{
    std::free(logo24);
    std::free(cursor_img);
}

void Cycle::update(std::vector<UI::Element> &elements, Framebuf &buf, input_event *iev, int fd_keyboard, int fd_mouse)
{
    if (read(fd_keyboard, iev, sizeof(*iev)) == (ssize_t)sizeof(*iev))
        ;

    while (read(fd_mouse, iev, sizeof(*iev)) == (ssize_t)sizeof(*iev))
        if (iev->type == EV_REL)
        {
            if (iev->code == REL_X)
                cursor.x += iev->value;
            else if (iev->code == REL_Y)
                cursor.y += iev->value;
        }
        else if (iev->type == EV_KEY)
            if (iev->value == 0)
                switch (iev->code)
                {
                case BTN_LEFT:
                    mouse_l = false;
                    break;
                case BTN_MIDDLE:
                    mouse_m = false;
                    break;
                case BTN_RIGHT:
                    mouse_r = false;
                    break;
                }
            else if (iev->value == 1)
                switch (iev->code)
                {
                case BTN_LEFT:
                    mouse_l = true;
                    break;
                case BTN_MIDDLE:
                    mouse_m = true;
                    break;
                case BTN_RIGHT:
                    mouse_r = true;
                    break;
                }

    std::uint32_t buf_w, buf_h;
    buf.size_get(&buf_w, &buf_h);
    if (cursor.x >= buf_w)
        cursor.x = buf_w - 1;
    if (cursor.x < 1)
        cursor.x = 1;
    if (cursor.y >= buf_h)
        cursor.y = buf_h - 1;
    if (cursor.y < 1)
        cursor.y = 1;

    for (std::size_t i = 0; i < elements.size(); i++)
        buf.el_update(elements.at(i));
    buf.el_update(quitbtn);
    for (std::size_t i = 0; i < windows.size(); i++)
        windows.at(i).update(buf, cursor);
    running = !quitbtn.button.active;
}

void Cycle::render(std::vector<UI::Element> &elements, Framebuf &buf)
{
    buf.px_clear(0xFF007FFF);

    for (std::size_t i = 0; i < elements.size(); i++)
        buf.px_render(elements.at(i));
    buf.px_render(quitbtn);
    for (std::size_t i = 0; i < windows.size(); i++)
        windows.at(i).render(buf);
    buf.px_render(cursor);
    buf.swap();
}
