#include <sys/ioctl.h>
#include <linux/kd.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>
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
    char devicepath_input[DEVICE_PATH_MAX];
    Keyboard::get(devicepath_input);

    int fd_input = open(devicepath_input, O_RDONLY | O_NONBLOCK);
    if (!fd_input)
    {
        ioctl(fd_console, KDSETMODE, KD_TEXT);
        close(fd_console);
        std::cerr << "Unable to launch gooner! No keyboard device was found." << std::endl;
        return 1;
    }

    std::vector<UI::Element> elements;
    if (!Cycle::init(elements, buf))
    {
        close(fd_console);
        return 1;
    }

    ioctl(fd_console, KDSETMODE, KD_GRAPHICS);
    struct input_event iev;
    while (Cycle::running)
    {
        Cycle::update(elements, &iev, fd_input);
        Cycle::render(elements, buf);
    }

    Cycle::cleanup();
    ioctl(fd_console, KDSETMODE, KD_TEXT);
    close(fd_console);
    return 0;
}
