#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "gnr.h"

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        printf("Missing executable! (hint: this error will soon be replaced by a simple desktop environment)\n\t%s [-tell] <executable.gnr> ...\n", argv[0]);
        return 1;
    }

    bool is_tell = strcmp(argv[1], "-tell") == 0 || strcmp(argv[1], "-t") == 0;
    if (is_tell && argc != 3)
    {
        printf("Missing executable! \n\t%s -tell <executable.gnr>\n", argv[0]);
        return 1;
    }

    FILE *executable = fopen(is_tell ? argv[2] : argv[1], "rb");
    if (!executable)
    {
        fputs("Failed to open executable!\n", stderr);
        perror("fopen");
        return 1;
    }

    if (!gnr_validate(executable))
    {
        fclose(executable);
        return 1;
    }

    GnrData data = {0};
    if (!gnr_tell(executable, &data))
    {
        fclose(executable);
        return 1;
    }

    if (is_tell)
    {
        char published[32];
        struct tm *tm = localtime(&data.published);
        strftime(published, sizeof(published), "%m/%d/%Y %H:%M", tm);

        printf("|----------------------------------------------------------------|\n");
        printf("| Executable Information (-tell)                                 |\n");
        printf("|----------------------------------------------------------------|\n");
        printf("| Compact: %s v%u.%u.%u by %s (published %s)\n", data.name, data.version.major, data.version.minor, data.version.patch, data.author, published);
        printf("|----------------------------------------------------------------|\n");
        printf("| Name: %s\n", data.name);
        printf("| Author: %s\n", data.author);
        printf("| Version: %u.%u.%u\n", data.version.major, data.version.minor, data.version.patch);
        printf("| Published: %s\n", published);
        printf("|----------------------------------------------------------------|\n");
    }
    else
    {
        if (!gnr_execute(executable, (char *const *)(argv + 1)))
        {
            fclose(executable);
            return 1;
        }
    }

    fclose(executable);
    return 0;
}
