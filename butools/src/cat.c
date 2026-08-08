#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096

static int cat(int fd, const char *name)
{
    char buf[BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buf, sizeof(buf))) > 0)
    {
        ssize_t bytes_written = 0;
        while (bytes_written < bytes_read)
        {
            ssize_t written = write(STDOUT_FILENO, buf + bytes_written, bytes_read - bytes_written);
            if (written < 0)
            {
                perror("write");
                return 1;
            }

            bytes_written += written;
        }
    }

    if (bytes_read < 0)
    {
        perror(name);
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc == 1)
        return cat(STDIN_FILENO, "stdin");

    int status = 0;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-") == 0)
        {
            if (cat(STDIN_FILENO, "stdin") != 0)
                status = 1;
        }
        else
        {
            int fd = open(argv[i], O_RDONLY);
            if (fd < 0)
            {
                perror(argv[i]);
                status = 1;
                continue;
            }

            if (cat(fd, argv[i]) != 0)
                status = 1;

            close(fd);
        }
    }

    return status;
}
