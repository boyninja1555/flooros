#include <linux/limits.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#define MAX_COMMAND_LENGTH 128
#define MAX_TOKENS 16
#define MAX_TOKEN_LENGTH 64

typedef enum
{
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_TERMINATE,
} TokenType;

typedef struct
{
    TokenType type;
    char *value;
} Token;

static char token_storage[MAX_TOKENS][MAX_TOKEN_LENGTH];

static int lex(char *command, Token tokens[])
{
    int token_count = 0;
    int storage_index = 0;
    bool in_string = false;
    bool reading = false;

    char temp[MAX_TOKEN_LENGTH];
    int temp_length = 0;
    for (int i = 0;; i++)
    {
        char c = command[i];

        if (c == '\0' || c == '\n')
        {
            if (in_string)
            {
                fprintf(stderr, "Unterminated string!\n");
                return -1;
            }

            if (reading)
            {
                if (token_count >= MAX_TOKENS)
                    return token_count;

                temp[temp_length] = '\0';
                memcpy(token_storage[storage_index], temp, temp_length + 1);
                tokens[token_count].type = TOKEN_WORD;
                tokens[token_count].value = token_storage[storage_index];
                token_count++;
                storage_index++;
            }

            tokens[token_count].type = TOKEN_TERMINATE;
            tokens[token_count].value = NULL;
            return token_count;
        }

        if (in_string)
        {
            if (c == '"')
            {
                temp[temp_length] = '\0';

                memcpy(token_storage[storage_index], temp, temp_length + 1);
                tokens[token_count].type = TOKEN_WORD;
                tokens[token_count].value = token_storage[storage_index];
                token_count++;
                storage_index++;
                temp_length = 0;
                reading = false;
                in_string = false;
            }
            else if (temp_length < MAX_TOKEN_LENGTH - 1)
                temp[temp_length++] = c;

            continue;
        }

        if (c == '"')
        {
            in_string = true;
            reading = true;
            continue;
        }

        if (c == '|')
        {
            if (reading)
            {
                temp[temp_length] = '\0';
                memcpy(token_storage[storage_index], temp, temp_length + 1);
                tokens[token_count].type = TOKEN_WORD;
                tokens[token_count].value = token_storage[storage_index];
                token_count++;
                storage_index++;
                temp_length = 0;
                reading = false;
            }

            if (token_count >= MAX_TOKENS || storage_index >= MAX_TOKENS)
                return token_count;

            token_storage[storage_index][0] = '|';
            token_storage[storage_index][1] = '\0';
            tokens[token_count].type = TOKEN_PIPE;
            tokens[token_count].value = token_storage[storage_index];
            token_count++;
            storage_index++;
            continue;
        }

        if (c == ' ' || c == '\t')
        {
            if (reading)
            {
                temp[temp_length] = '\0';

                memcpy(token_storage[storage_index], temp, temp_length + 1);
                tokens[token_count].type = TOKEN_WORD;
                tokens[token_count].value = token_storage[storage_index];
                token_count++;
                storage_index++;
                temp_length = 0;
                reading = false;
            }

            continue;
        }

        reading = true;
        if (temp_length < MAX_TOKEN_LENGTH - 1)
            temp[temp_length++] = c;
    }
}

static int spawn_process(char *cwd, int argc, char *argv[])
{
    if (argc == 0)
        return 0;

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

    pid_t pid = fork();
    if (pid == 0)
    {
        execv(argv[0], argv);
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

static int execute_pipeline(char *cwd, int cmds_num, char *cmds_argv[][MAX_TOKENS + 1])
{
    int pipefds[2 * (cmds_num - 1)];
    for (int i = 0; i < cmds_num - 1; i++)
    {
        if (pipe(pipefds + i * 2) < 0)
        {
            perror("pipe");
            return 1;
        }
    }

    pid_t pids[cmds_num];
    for (int i = 0; i < cmds_num; i++)
    {
        pids[i] = fork();
        if (pids[i] == 0)
        {
            if (i > 0)
            {
                if (dup2(pipefds[(i - 1) * 2], STDIN_FILENO) < 0)
                {
                    perror("dup2 stdin");
                    _exit(1);
                }
            }

            if (i < cmds_num - 1)
                if (dup2(pipefds[i * 2 + 1], STDOUT_FILENO) < 0)
                {
                    perror("dup2 stdout");
                    _exit(1);
                }

            for (int j = 0; j < 2 * (cmds_num - 1); j++)
                close(pipefds[j]);

            if (strcmp(cmds_argv[i][0], "pwd") == 0)
            {
                printf("%s\n", cwd);
                _exit(0);
            }

            if (strcmp(cmds_argv[i][0], "cd") == 0)
                _exit(0);

            execv(cmds_argv[i][0], cmds_argv[i]);
            perror("execv");
            _exit(1);
        }
        else if (pids[i] < 0)
        {
            perror("fork");
            return 1;
        }
    }

    for (int i = 0; i < 2 * (cmds_num - 1); i++)
        close(pipefds[i]);

    int last_status = 0;
    for (int i = 0; i < cmds_num; i++)
    {
        int status;
        waitpid(pids[i], &status, 0);
        if (i == cmds_num - 1)
            last_status = status;
    }

    return last_status;
}

int main()
{
    char cwd[PATH_MAX];
    char hostname[64];
    while (1)
    {
        getcwd(cwd, sizeof(cwd));
        cwd[sizeof(cwd) - 1] = '\0';

        gethostname(hostname, sizeof(hostname));
        printf("\x1b[1;32msf@%s\x1b[0m:\x1b[1;34m%s\x1b[0m$ ", hostname, cwd);
        fflush(stdout);

        char command[MAX_COMMAND_LENGTH];
        if (!fgets(command, sizeof(command), stdin))
            break;

        if (strchr(command, '\n') == NULL)
        {
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
        }

        Token tokens[MAX_TOKENS];
        int token_count = lex(command, tokens);
        if (token_count <= 0)
            continue;

        char *cmds_argv[MAX_TOKENS][MAX_TOKENS + 1];
        int cmds_argc[MAX_TOKENS] = {0};
        int cmds_num = 0;
        int current_arg = 0;
        bool syntax_error = false;

        for (int i = 0; i < token_count; i++)
        {
            if (tokens[i].type == TOKEN_PIPE)
            {
                if (current_arg == 0)
                {
                    fprintf(stderr, "Ssyntax error near unexpected token '|'!\n");
                    syntax_error = true;
                    break;
                }

                cmds_argv[cmds_num][current_arg] = NULL;
                cmds_argc[cmds_num] = current_arg;
                cmds_num++;
                current_arg = 0;
            }
            else if (tokens[i].type == TOKEN_WORD)
                cmds_argv[cmds_num][current_arg++] = tokens[i].value;
        }

        if (syntax_error)
            continue;

        if (current_arg == 0)
        {
            if (cmds_num > 0)
            {
                fprintf(stderr, "sf: syntax error near unexpected token '|'\n");
                continue;
            }
        }
        else
        {
            cmds_argv[cmds_num][current_arg] = NULL;
            cmds_argc[cmds_num] = current_arg;
            cmds_num++;
        }

        if (cmds_num == 0)
            continue;

        if (cmds_num == 1 && strcmp(cmds_argv[0][0], "exit") == 0)
        {
            int status = 0;
            if (cmds_argc[0] > 1)
                status = atoi(cmds_argv[0][1]);

            return status;
        }

        if (cmds_num == 1)
            spawn_process(cwd, cmds_argc[0], cmds_argv[0]);
        else
            execute_pipeline(cwd, cmds_num, cmds_argv);
    }

    return 0;
}
