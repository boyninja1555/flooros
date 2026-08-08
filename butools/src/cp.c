#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <libgen.h>
#include <stdbool.h>

#define BUF_SIZE 8192

static int copy_file(const char *source, const char *destination)
{
    struct stat st;
    if (lstat(source, &st) != 0)
    {
        perror(source);
        return 1;
    }

    if (S_ISDIR(st.st_mode))
    {
        fprintf(stderr, "%s is a directory!\n", source);
        return 1;
    }

    int in_fd = open(source, O_RDONLY);
    if (in_fd < 0)
    {
        perror(source);
        return 1;
    }

    int out_fd = open(destination, O_WRONLY | O_CREAT | O_TRUNC, st.st_mode & 0777);
    if (out_fd < 0)
    {
        perror(destination);
        close(in_fd);
        return 1;
    }

    char buf[BUF_SIZE];
    ssize_t nread;
    int status = 0;
    while ((nread = read(in_fd, buf, sizeof(buf))) > 0)
    {
        ssize_t nwritten = 0;
        while (nwritten < nread)
        {
            ssize_t written = write(out_fd, buf + nwritten, nread - nwritten);
            if (written < 0)
            {
                perror(destination);
                status = 1;
                break;
            }

            nwritten += written;
        }

        if (status != 0)
            break;
    }

    if (nread < 0)
    {
        perror(source);
        status = 1;
    }

    close(in_fd);
    close(out_fd);
    return status;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <source...> <destination>\n", argv[0]);
        return 1;
    }

    const char *target = argv[argc - 1];
    struct stat target_st;
    bool target_isd = (stat(target, &target_st) == 0 && S_ISDIR(target_st.st_mode));
    if (argc > 3 && !target_isd)
    {
        fprintf(stderr, "Target \"%s\" is not a directory!\n", target);
        return 1;
    }

    int status = 0;
    for (int i = 1; i < argc - 1; i++)
    {
        char destination[PATH_MAX];
        if (target_isd)
        {
            char source_copy[PATH_MAX];
            strncpy(source_copy, argv[i], sizeof(source_copy) - 1);
            source_copy[sizeof(source_copy) - 1] = '\0';
            snprintf(destination, sizeof(destination), "%s/%s", target, basename(source_copy));
        }
        else
            snprintf(destination, sizeof(destination), "%s", target);

        if (copy_file(argv[i], destination) != 0)
            status = 1;
    }

    return status;
}
