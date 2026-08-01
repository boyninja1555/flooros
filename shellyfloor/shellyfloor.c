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

int main()
{
    char cwd[4096];
    while (1)
    {
        getcwd(cwd, sizeof(cwd));
        cwd[sizeof(cwd) - 1] = '\0';

        printf("sf@FloorOS:%s$ ", cwd);
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

        char *argv[MAX_TOKENS];
        for (int i = 0; i < token_count; i++)
            argv[i] = tokens[i].value;

        argv[token_count] = NULL;

        if (strcmp(argv[0], "exit") == 0)
        {
            int status = 0;
            if (token_count > 1)
                status = atoi(argv[1]);

            return status;
        }

        int status = spawn_process(cwd, token_count, argv);
        if (status != 0)
            printf("Process exited with status %d\n", WEXITSTATUS(status));
    }

    return 0;
}
