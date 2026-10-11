#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <linux/kd.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>
#include "cycle.hpp"
#include "server.hpp"
#include "fb.hpp"
#include "kb.hpp"

static Framebuf buf;

int main(int argc, const char *argv[])
{
    if (argc == 2 && std::string(argv[1]) == "-welcome")
    {
        int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (client_fd < 0)
        {
            std::perror("socket");
            return 1;
        }

        sockaddr_un addr{};
        addr.sun_family = AF_UNIX;

        const char *sockfile = "/tmp/gooner.sock";
        std::strncpy(addr.sun_path, sockfile, sizeof(addr.sun_path) - 1);

        bool connected = false;
        for (int i = 0; i < 100; i++)
        {
            if (connect(client_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == 0)
            {
                connected = true;
                break;
            }

            if (errno != ENOENT && errno != ECONNREFUSED)
                break;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        if (!connected)
        {
            std::perror("connect");
            close(client_fd);
            return 1;
        }

        GoonerServer::Action action = GoonerServer::Action::OPEN;
        send(client_fd, &action, sizeof(std::uint8_t), 0);

        std::string title = "WELCOME";
        std::size_t titlelen = title.length();
        send(client_fd, &titlelen, sizeof(std::size_t), 0);
        send(client_fd, title.c_str(), titlelen, 0);

        std::uint32_t x = 0, y = 0;
        send(client_fd, &x, sizeof(std::uint32_t), 0);
        send(client_fd, &y, sizeof(std::uint32_t), 0);

        std::uint32_t w = 800, h = 500;
        send(client_fd, &w, sizeof(std::uint32_t), 0);
        send(client_fd, &h, sizeof(std::uint32_t), 0);

        std::uint8_t flags = WINDOW_CENTERED;
        send(client_fd, &flags, sizeof(std::uint8_t), 0);
        return 0;
    }

    buf = Framebuf();
    buf.init();

    int fd_console = open("/dev/console", O_RDWR);
    if (fd_console < 0)
    {
        std::cerr << "Unable to launch gooner! Console did not open." << std::endl;
        perror("open");
        return 1;
    }

    char devicepath_keyboard[DEVICE_PATH_MAX];
    char devicepath_mouse[DEVICE_PATH_MAX];
    Keyboard::get(devicepath_keyboard);
    Mouse::get(devicepath_mouse);

    int fd_keyboard = open(devicepath_keyboard, O_RDONLY | O_NONBLOCK);
    if (fd_keyboard < 0)
    {
        close(fd_console);
        std::cerr << "Unable to launch gooner! No keyboard device was found." << std::endl;
        perror("open");
        return 1;
    }

    int fd_mouse = open(devicepath_mouse, O_RDONLY | O_NONBLOCK);
    if (fd_mouse < 0)
    {
        close(fd_console);
        close(fd_keyboard);
        std::cerr << "Unable to launch gooner! No mouse device was found." << std::endl;
        perror("open");
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
