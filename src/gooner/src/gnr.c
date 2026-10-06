#define _GNU_SOURCE
#include "gnr.h"
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "gnrmake.h"

int main_gnr(int argc, const char *argv[])
{
    bool is_make = strcmp(argv[1], "-make") == 0 || strcmp(argv[1], "-m") == 0;
    if (is_make)
    {
        if (argc != 3)
        {
            printf("Missing config!\n\t%s -make <config.xml>\n", argv[0]);
            return 1;
        }

        return gnr_make(argv[2]);
    }

    bool is_tell = strcmp(argv[1], "-tell") == 0 || strcmp(argv[1], "-t") == 0;
    if (is_tell && argc != 3)
    {
        printf("Missing executable!\n\t%s -tell <executable.gnr>\n", argv[0]);
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

void gnrdata_free(GnrData *data)
{
    if (!data)
        return;
    free(data->name);
    free(data->author);
    data->published = 0;
}

bool gnr_validate(FILE *file)
{
    if (!file)
    {
        fputs("Cannot validate a non-existent executable!\n", stderr);
        return false;
    }

    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (fsize < GNR_MAGIC_LENGTH)
    {
        fprintf(stderr, "Executable must be at least %u bytes to pass validation!\n", GNR_MAGIC_LENGTH);
        return false;
    }

    char magic[GNR_MAGIC_LENGTH];
    fread(magic, 1, GNR_MAGIC_LENGTH, file);
    if (memcmp(GNR_MAGIC, magic, GNR_MAGIC_LENGTH) != 0)
    {
        fputs("File is not a valid executable! Check for obvious corruption and/or typos.\n", stderr);
        return false;
    }

    uint16_t gnr_version;
    fread(&gnr_version, 1, sizeof(uint16_t), file);

    if (gnr_version > GNR_VERSION)
    {
        fprintf(stderr, "Executable built for a newer version of gooner's GNR feature! (fv%u)\n\tYour installation only supports fv%u.\n", gnr_version, GNR_VERSION);
        return false;
    }

    if (gnr_version < GNR_VERSION)
    {
        fprintf(stderr, "Executable built for an older version of gooner's GNR feature! (fv%u)\n\tYour installation only supports fv%u.\n", gnr_version, GNR_VERSION);
        return false;
    }

    return true;
}

bool gnr_tell(FILE *file, GnrData *data)
{
    if (!file)
    {
        fputs("Cannot tell a non-existent executable!\n", stderr);
        return false;
    }

    size_t name_length = 0;
    size_t name_cap = 1;
    data->name = malloc(name_cap);
    data->name[0] = '\0';

    char name_c = '@';
    while (name_c != '\0')
    {
        fread(&name_c, 1, 1, file);

        if (name_length >= name_cap)
        {
            name_cap *= 2;
            char *n = realloc(data->name, name_cap);
            if (!n)
            {
                fputs("Ran out of memory while building the name!\n", stderr);
                return false;
            }

            data->name = n;
        }

        data->name[name_length++] = name_c;
    }

    size_t author_length = 0;
    size_t author_cap = 1;
    data->author = malloc(author_cap);
    data->author[0] = '\0';

    char author_c = '@';
    while (author_c != '\0')
    {
        fread(&author_c, 1, 1, file);

        if (author_length >= author_cap)
        {
            author_cap *= 2;
            char *n = realloc(data->author, author_cap);
            if (!n)
            {
                fputs("Ran out of memory while building the author!\n", stderr);
                return false;
            }

            data->author = n;
        }

        data->author[author_length++] = author_c;
    }

    data->version.major = 0;
    data->version.minor = 0;
    data->version.patch = 0;
    fread(&data->version.major, 1, sizeof(uint8_t), file);
    fread(&data->version.minor, 1, sizeof(uint8_t), file);
    fread(&data->version.patch, 1, sizeof(uint8_t), file);

    data->published = 0;
    fread(&data->published, 1, sizeof(time_t), file);
    return true;
}

bool gnr_execute(FILE *file, char *const *argv)
{
    if (!file)
    {
        fputs("Cannot execute a non-existent executable!\n", stderr);
        return false;
    }

    long elf_start = ftell(file);
    fseek(file, 0, SEEK_END);
    long elf_size = ftell(file);
    fseek(file, elf_start, SEEK_SET);

    char *elf = malloc(elf_size);
    fread(elf, 1, elf_size, file);

    int fd = memfd_create("gooner", 0);
    if (fd == -1)
    {
        perror("memfd_create");
        free(elf);
        return false;
    }

    write(fd, elf, elf_size);
    execveat(fd, "", argv, (char *[]){NULL}, AT_EMPTY_PATH);
    perror("execveat");
    free(elf);
    return false;
}
