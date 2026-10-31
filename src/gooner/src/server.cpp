#include "server.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <string>
#include "cycle.hpp"

GoonerServer::Instance::Instance() : running(false)
{
}

GoonerServer::Instance::~Instance()
{
}

void GoonerServer::Instance::start()
{
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        return;
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;

    const char *sockfile = "/tmp/gooner.sock";
    std::strncpy(addr.sun_path, sockfile, sizeof(addr.sun_path) - 1);
    unlink(sockfile);

    if (bind(server_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
    {
        perror("bind");
        close(server_fd);
        return;
    }

    if (listen(server_fd, 16) < 0)
    {
        perror("listen");
        close(server_fd);
        unlink(sockfile);
        return;
    }

    running = true;
    long idc = 0L;
    while (running)
    {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0)
        {
            perror("accept");
            continue;
        }

        while (true)
        {
            GoonerServer::Action action{};
            ssize_t actionsize = recv(client_fd, &action, sizeof(std::uint8_t), MSG_WAITALL);
            if (actionsize == 0 || actionsize != sizeof(action))
                break;
            switch (action)
            {
            case GoonerServer::Action::OPEN:
            {
                std::size_t titlelen;
                recv(client_fd, &titlelen, sizeof(std::size_t), MSG_WAITALL);

                std::string title(titlelen, '\0');
                if (recv(client_fd, title.data(), titlelen, MSG_WAITALL) != static_cast<ssize_t>(titlelen))
                    break;

                std::uint32_t x, y;
                recv(client_fd, &x, sizeof(std::uint32_t), MSG_WAITALL);
                recv(client_fd, &y, sizeof(std::uint32_t), MSG_WAITALL);

                std::uint32_t w, h;
                recv(client_fd, &w, sizeof(std::uint32_t), MSG_WAITALL);
                recv(client_fd, &h, sizeof(std::uint32_t), MSG_WAITALL);

                std::uint8_t flags;
                recv(client_fd, &flags, sizeof(std::uint8_t), MSG_WAITALL);

                auto window = Window(idc, title, x, y, w, h, flags);
                Cycle::queue_window(window);
                send(client_fd, &idc, sizeof(long), 0);
                idc++;
                break;
            }

            case GoonerServer::Action::CLOSE:
            {
                long id;
                recv(client_fd, &id, sizeof(long), MSG_WAITALL);
                if (id < 0 || !Cycle::windows.contains(id))
                    break;
                Cycle::windows.at(id).close();
                break;
            }

            case GoonerServer::Action::PAINT:
            {
                long id;
                recv(client_fd, &id, sizeof(long), MSG_WAITALL);
                if (id < 0 || !Cycle::windows.contains(id))
                    break;

                Window &window = Cycle::windows.at(id);
                recv(client_fd, reinterpret_cast<char *>(window.winbuf.data()), sizeof(std::uint32_t) * window.winbuf.size(), MSG_WAITALL);
                break;
            }

            default:
                goto cdone;
            }
        }
    cdone:
        close(client_fd);
    }

    close(server_fd);
    unlink(sockfile);
}

void GoonerServer::Instance::shutoff()
{
    running = false;
}
