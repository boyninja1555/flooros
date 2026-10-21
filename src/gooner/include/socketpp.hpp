#pragma once

#include <sys/types.h>

ssize_t sendall(int fd, const void *buf, size_t n, int flags);

ssize_t recvall(int fd, void *buf, size_t n, int flags);
