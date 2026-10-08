#include <sys/ioctl.h>
#include <linux/kd.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <vector>
#include <iostream>
#include "fb.hpp"
#include "glyph.hpp"
#include "image.hpp"
#include "kb.hpp"

#define TASKBAR_H 32

static bool running = true;
static Framebuf buf;

void update(std::vector<UI::Element> &elements, input_event *iev, int fd_input)
{
    if (read(fd_input, iev, sizeof(*iev)) == (ssize_t)sizeof(*iev))
        if (iev->type == EV_KEY && iev->value == 1 && iev->code == KEY_Q)
            running = false;
}

void render(std::vector<UI::Element> &elements, Framebuf &buf)
{
    buf.px_clear(0xFF007FFF);
    for (std::size_t i = 0; i < elements.size(); i++)
        buf.px_render(elements.at(i));
    buf.swap();
}

int main()
{
    buf = Framebuf();
    buf.init();

    std::uint32_t buf_w, buf_h;
    buf.size_get(&buf_w, &buf_h);
    if (buf_w == 0 || buf_h == 0)
    {
        std::cerr << "Unable to launch gooner! Buffer initialization did not succeed, likely due to a missing graphics option on your system." << std::endl;
        return 1;
    }

    UI::color_t *logo;
    uint32_t logo_w, logo_h;
    ImageLoad::ppm("/etc/logo24.ppm", &logo, &logo_w, &logo_h);
    if (!logo || logo_w == 0 || logo_h == 0)
    {
        logo_w = 24, logo_h = 24;
        size_t logo_size = logo_w * logo_h * 4;
        logo = (UI::color_t *)std::malloc(logo_size);
        std::memset(logo, 0, logo_size);
        std::cerr << "Logo image (/etc/logo24.ppm) failed to load! Using default image data." << std::endl;
    }

    int fd_console = open("/dev/console", O_RDWR);
    ioctl(fd_console, KDSETMODE, KD_GRAPHICS);

    char devicepath_input[DEVICE_PATH_MAX];
    Keyboard::get(devicepath_input);

    int fd_input = open(devicepath_input, O_RDONLY | O_NONBLOCK);
    if (!fd_input)
    {
        std::cerr << "Unable to launch gooner! No keyboard device was found." << std::endl;
        return 1;
    }

    std::vector<UI::Element> elements;
    elements.push_back(UI::rect(0, buf_h - TASKBAR_H, buf_w, TASKBAR_H, 0xFF0040FF));
    elements.push_back(UI::image(4, buf_h - TASKBAR_H + TASKBAR_H / 2 - logo_h / 2, logo_w, logo_h, logo));
    elements.push_back(UI::text(4 + logo_w + 4, buf_h - TASKBAR_H + TASKBAR_H / 2 - FONTC_HEIGHT / 2, "FLOOROS", 0xFFDEDEDE));

    UI::Element text_quitmsg = UI::text(0, buf_h - TASKBAR_H + TASKBAR_H / 2 - FONTC_HEIGHT / 2, "PRESS Q TO QUIT", 0xFF9F9F9F);
    text_quitmsg.x = buf_w - 4 - FONTC_WIDTH * strlen(text_quitmsg.text.text);
    elements.push_back(text_quitmsg);

    // Temporary
    elements.push_back(UI::text(4, 4, "WELCOME TO THE FLOOROS GOONER DESKTOP!", 0xFFDEDEDE));
    elements.push_back(UI::text(4, 8 + FONTC_HEIGHT, "AS OF NOW, THERE IS NOT MUCH TO SEE HERE.", 0xFFDEDEDE));
    elements.push_back(UI::text(4, 12 + FONTC_HEIGHT * 2, "THE GITHUB HAS A TON OF COMMIT MESSAGES THAT MAY OR MAY NOT RELATE TO GOONER UPDATES.", 0xFFDEDEDE));
    elements.push_back(UI::text(4, 16 + FONTC_HEIGHT * 3, "ENJOY QUITTING!", 0xFFDEDEDE));

    struct input_event iev;
    while (running)
    {
        update(elements, &iev, fd_input);
        render(elements, buf);
    }

    free(logo);
    ioctl(fd_console, KDSETMODE, KD_TEXT);
    close(fd_console);
    return 0;
}
