#include <linux/limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

static bool input_yn(const char *question, bool def)
{
    char buf[16];
    printf("%s (%c/%c) ", question, def ? 'Y' : 'y', def ? 'n' : 'N');
    fflush(stdout);
    if (fgets(buf, sizeof(buf), stdin) != NULL)
    {
        if (buf[0] == 'y' || buf[0] == 'Y')
            return true;
        else if (buf[0] == 'n' || buf[0] == 'N')
            return false;
        else
            return def;
    }

    return def;
}

static int remove_recursive(const char *path)
{
    struct stat st;
    if (lstat(path, &st) != 0)
    {
        perror(path);
        return 1;
    }

    char msg[PATH_MAX + 64];
    if (!S_ISDIR(st.st_mode))
    {
        snprintf(msg, sizeof(msg), "Delete file \"%s\"?", path);
        if (!input_yn(msg, false))
            return 0;

        if (unlink(path) != 0)
        {
            perror(path);
            return 1;
        }

        return 0;
    }

    DIR *directory = opendir(path);
    if (!directory)
    {
        perror(path);
        return 1;
    }

    struct dirent *entry;
    int status = 0;
    while ((entry = readdir(directory)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char subpath[PATH_MAX];
        int len = snprintf(subpath, sizeof(subpath), "%s/%s", path, entry->d_name);
        if (len < 0 || (size_t)len >= sizeof(subpath))
        {
            fprintf(stderr, "Path exceeds maximum length! %s/%s\n", path, entry->d_name);
            status = 1;
            continue;
        }

        if (remove_recursive(subpath) != 0)
            status = 1;
    }

    closedir(directory);
    snprintf(msg, sizeof(msg), "Delete directory \"%s\"?", path);
    if (!input_yn(msg, false))
        return status;

    if (rmdir(path) != 0)
    {
        perror(path);
        return 1;
    }

    return status;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s [-r] <path...>\n", argv[0]);
        return 1;
    }

    bool recursive = false;
    int start_index = 1;
    if (strcmp(argv[1], "-r") == 0)
    {
        recursive = true;
        start_index = 2;
        if (argc < 3)
        {
            fprintf(stderr, "Usage: %s [-r] <path...>\n", argv[0]);
            return 1;
        }
    }

    int status = 0;
    for (int i = start_index; i < argc; i++)
    {
        struct stat st;
        if (lstat(argv[i], &st) != 0)
        {
            perror(argv[i]);
            status = 1;
            continue;
        }

        if (S_ISDIR(st.st_mode))
        {
            if (!recursive)
            {
                fprintf(stderr, "%s is a directory! Use -r to delete it recursively.\n", argv[i]);
                status = 1;
            }
            else
            {
                if (remove_recursive(argv[i]) != 0)
                    status = 1;
            }
        }
        else
        {
            char msg[PATH_MAX + 64];
            snprintf(msg, sizeof(msg), "Delete file \"%s\"?", argv[i]);
            if (!input_yn(msg, false))
                continue;

            if (unlink(argv[i]) != 0)
            {
                perror(argv[i]);
                status = 1;
            }
        }
    }

    return status;
}
