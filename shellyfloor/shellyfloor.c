#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int spawn_process(char *cwd, int argc, char *argv[])
{
    // Shell builtins

    if (strcmp(argv[0], "cd") == 0)
    {
        if (argc < 2)
        {
            fprintf(stderr, "cd: missing path argument\n");
            return 1;
        }

        if (chdir(argv[1]) != 0)
        {
            perror("cd");
            return 1;
        }

        return 0;
    }

    if (strcmp(argv[0], "pwd") == 0)
    {
        printf("%s\n", cwd);
        return 0;
    }

    // Actual processes

    pid_t pid = fork();
    if (pid == 0)
    {
        chdir(cwd);
        execv(argv[0], (char *const *)argv);
        perror("execv");
        _exit(1);
    }

    if (pid > 0)
    {
        int status;
        waitpid(pid, &status, 0);
        return status;
    }

    perror("fork");
    return 1;
}

int main(int argc, char *argv[])
{
    char cwd[4096];
    while (1)
    {
        getcwd(cwd, sizeof(cwd));
        cwd[sizeof(cwd) - 1] = '\0';

        printf("sf@FloorOS:%s$ ", cwd);
        fflush(stdout);

        char command[128];
        if (fgets(command, sizeof(command), stdin))
        {
            size_t len = strlen(command);
            if (len > 0 && command[len - 1] == '\n')
                command[len - 1] = '\0';

            int process_argc = 0;
            char *process_argv[17];
            while (process_argc < 17)
            {
                char *token = strtok(process_argc == 0 ? command : NULL, " ");
                if (!token)
                    break;

                process_argv[process_argc++] = token;
            }

            process_argv[process_argc] = NULL;

            if (strcmp(process_argv[0], "exit") == 0)
            {
                int status = 0;
                if (process_argc > 1)
                    status = atoi(process_argv[1]);

                return status;
            }

            int status = spawn_process(cwd, process_argc, process_argv);
            if (status != 0)
                printf("Process exited with status %d\n", WEXITSTATUS(status));
        }
    }

    return 1;
}
