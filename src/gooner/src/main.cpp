#include <sys/ioctl.h>
#include <linux/kd.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>
#include "cycle.hpp"
#include "fb.hpp"
#include "kb.hpp"

static Framebuf buf;

int main()
{
    buf = Framebuf();
    buf.init();

    int fd_console = open("/dev/console", O_RDWR);

    char devicepath_keyboard[DEVICE_PATH_MAX];
    char devicepath_mouse[DEVICE_PATH_MAX];
    Keyboard::get(devicepath_keyboard);
    Mouse::get(devicepath_mouse);

    int fd_keyboard = open(devicepath_keyboard, O_RDONLY | O_NONBLOCK);
    if (!fd_keyboard)
    {
        close(fd_console);
        std::cerr << "Unable to launch gooner! No keyboard device was found." << std::endl;
        return 1;
    }

    int fd_mouse = open(devicepath_mouse, O_RDONLY | O_NONBLOCK);
    if (!fd_mouse)
    {
        close(fd_console);
        close(fd_keyboard);
        std::cerr << "Unable to launch gooner! No mouse device was found." << std::endl;
        return 1;
    }

    std::vector<UI::Element> elements;
    if (!Cycle::init(elements, buf))
    {
        close(fd_console);
        close(fd_keyboard);
        close(fd_mouse);
        return 1;
    }

    ioctl(fd_console, KDSETMODE, KD_GRAPHICS);
    struct input_event iev;

    using namespace std::chrono_literals;
    auto next = std::chrono::steady_clock::now();
    constexpr auto frame_time = 16'666'667ns;
    while (Cycle::running)
    {
        Cycle::update(elements, buf, &iev, fd_keyboard, fd_mouse);
        Cycle::render(elements, buf);

        next += frame_time;
        std::this_thread::sleep_until(next);
    }

    Cycle::cleanup();
    ioctl(fd_console, KDSETMODE, KD_TEXT);
    close(fd_console);
    close(fd_keyboard);
    close(fd_mouse);
    return 0;
}
