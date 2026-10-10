#include "server.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <string>
#include "cycle.hpp"

static bool running = true;

GoonerServer::Instance::Instance()
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

    long idc = 0L;
    while (running)
    {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0)
        {
            perror("accept");
            continue;
        }

        GoonerServer::Action action;
        switch (action)
        {
        case GoonerServer::Action::OPEN:
        {
            size_t titlelen;
            recv(client_fd, &titlelen, sizeof(size_t), 0);

            std::string title;
            title.resize_and_overwrite(50, [client_fd](char *titleptr, std::size_t maxlen) -> std::size_t
                                       {
                                        ssize_t received = recv(client_fd, titleptr, maxlen, 0);
                                        if (received < 0) return 0; 
                                        return static_cast<std::size_t>(received); });

            std::uint32_t x, y;
            recv(client_fd, &x, sizeof(std::uint32_t), 0);
            recv(client_fd, &y, sizeof(std::uint32_t), 0);

            std::uint32_t w, h;
            recv(client_fd, &w, sizeof(std::uint32_t), 0);
            recv(client_fd, &h, sizeof(std::uint32_t), 0);

            std::uint8_t flags;
            recv(client_fd, &flags, sizeof(std::uint8_t), 0);

            Cycle::windows.try_emplace(idc, idc, title, x, y, w, h, flags);
            send(client_fd, &idc, sizeof(long), 0);
            idc++;
            break;
        }

        case GoonerServer::Action::CLOSE:
        {
            long id;
            recv(client_fd, &id, sizeof(long), 0);
            if (id < 0)
                break;
            Cycle::windows.at(id).close();
            break;
        }
        }

        close(client_fd);
    }

    close(server_fd);
    unlink(sockfile);
}

void GoonerServer::Instance::shutoff()
{
    running = false;
}
