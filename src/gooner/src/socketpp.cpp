#include "socketpp.hpp"
#include <sys/socket.h>
#include <cerrno>

ssize_t sendall(int fd, const void *buf, size_t n, int flags)
{
    const char *ptr = static_cast<const char *>(buf);
    size_t total = 0;
    while (total < n)
    {
        ssize_t n = send(fd, ptr + total, n - total, flags);

        if (n < 0)
        {
            if (errno == EINTR)
                continue;
            return -1;
        }

        if (n == 0)
            break;

        total += static_cast<size_t>(n);
    }

    return static_cast<ssize_t>(total);
}

ssize_t recvall(int fd, void *buf, size_t n, int flags)
{
    char *ptr = static_cast<char *>(buf);
    size_t total = 0;
    while (total < n)
    {
        ssize_t n = recv(fd, ptr + total, n - total, flags);

        if (n < 0)
        {
            if (errno == EINTR)
                continue;
            return -1;
        }

        if (n == 0)
            break;

        total += static_cast<size_t>(n);
    }

    return static_cast<ssize_t>(total);
}
