#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <libgen.h>
#include <stdbool.h>

static int move_item(const char *source, const char *destination)
{
    if (rename(source, destination) == 0)
        return 0;

    perror(source);
    return 1;
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

        if (move_item(argv[i], destination) != 0)
            status = 1;
    }

    return status;
}
