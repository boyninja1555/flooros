#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <path...>\n", argv[0]);
        return 1;
    }

    int status = 0;
    for (int i = 1; i < argc; i++)
    {
        int fd = open(argv[i], O_WRONLY | O_CREAT, 0666);
        if (fd == -1)
        {
            perror(argv[i]);
            status = 1;
            continue;
        }

        close(fd);
    }

    return status;
}
