#include <linux/limits.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct
{
    char name[PATH_MAX];
    int is_directory;
} Entry;

int compare_entries(const void *a, const void *b)
{
    const Entry *ea = a;
    const Entry *eb = b;
    if (ea->is_directory != eb->is_directory)
        return eb->is_directory - ea->is_directory;

    return strcmp(ea->name, eb->name);
}

int main(int argc, char *argv[])
{
    char *dirpath = ".";
    if (argc > 1)
        dirpath = argv[1];

    DIR *directory = opendir(dirpath);
    if (!directory)
    {
        perror("opendir");
        return 1;
    }

    Entry entries[256];
    int count = 0;
    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL && count < 256)
    {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(entries[count].name, sizeof(entries[count].name), "%s", entry->d_name);

        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/%s", argv[1], entry->d_name);

        struct stat info;
        entries[count].is_directory = 0;
        if (stat(path, &info) == 0)
            entries[count].is_directory = S_ISDIR(info.st_mode);

        count++;
    }

    closedir(directory);
    qsort(entries, count, sizeof(Entry), compare_entries);

    for (int i = 0; i < count; i++)
        if (entries[i].is_directory)
            printf("%s/\n", entries[i].name);
        else
            printf("%s\n", entries[i].name);

    return 0;
}
