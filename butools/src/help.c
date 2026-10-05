#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define MIN_LENGTH_LEFTHAND 16

int document(const char *command)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        char helpfile[7 + strlen(command) + 1];
        memcpy(helpfile, "/bin-h/", 7);
        strcpy(helpfile + 7, command);

        char *args[] = {"/bin/cat", helpfile};
        execv(args[0], args);
    }

    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    int status;
    if (waitpid(pid, &status, 0) < 0)
    {
        perror("waitpid");
        return 1;
    }

    return status;
}

void table_row(const char *command)
{
    size_t command_length = strlen(command);
    size_t spaces = MIN_LENGTH_LEFTHAND - command_length;

    char commandf[command_length + spaces + 1];
    memcpy(commandf, command, command_length);
    for (size_t i = 0; i < spaces; i++)
        commandf[command_length + i] = ' ';
    commandf[command_length + spaces] = '\0';
    printf("| %s | ", commandf);

    char helpfile[command_length + 8];
    memcpy(helpfile, "/bin-h/", 7);
    strcpy(helpfile + 7, command);

    struct stat st;
    if (lstat(helpfile, &st) != 0)
        puts("No                |");
    else
        puts("Yes               |");
}

int main(int argc, const char *argv[])
{
    if (argc > 1)
        return document(argv[1]);

    puts("|------------------|-------------------|");
    puts("| Command          | Has Documentation |");
    puts("|------------------|-------------------|");

    DIR *bindir = opendir("/bin");
    if (!bindir)
    {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(bindir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;
        table_row(entry->d_name);
    }

    closedir(bindir);
    puts("|------------------|-------------------|");
    return 0;
}
