#include <linux/limits.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
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
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_REDIRECT_APPEND,
    TOKEN_TERMINATE,
} TokenType;

typedef struct
{
    TokenType type;
    char *value;
} Token;

typedef struct
{
    char *argv[MAX_TOKENS + 1];
    int argc;
    char *infile;
    char *outfile;
    bool append_out;
} CommandStage;

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

        if (c == '<')
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

            token_storage[storage_index][0] = '<';
            token_storage[storage_index][1] = '\0';
            tokens[token_count].type = TOKEN_REDIRECT_IN;
            tokens[token_count].value = token_storage[storage_index];
            token_count++;
            storage_index++;
            continue;
        }

        if (c == '>')
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

            if (command[i + 1] == '>')
            {
                i++;
                token_storage[storage_index][0] = '>';
                token_storage[storage_index][1] = '>';
                token_storage[storage_index][2] = '\0';
                tokens[token_count].type = TOKEN_REDIRECT_APPEND;
            }
            else
            {
                token_storage[storage_index][0] = '>';
                token_storage[storage_index][1] = '\0';
                tokens[token_count].type = TOKEN_REDIRECT_OUT;
            }

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

static void apply_redirections(CommandStage *stage)
{
    if (stage->infile != NULL)
    {
        int fd = open(stage->infile, O_RDONLY);
        if (fd < 0)
        {
            perror(stage->infile);
            _exit(1);
        }

        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    if (stage->outfile != NULL)
    {
        int flags = O_WRONLY | O_CREAT | (stage->append_out ? O_APPEND : O_TRUNC);
        int fd = open(stage->outfile, flags, 0644);
        if (fd < 0)
        {
            perror(stage->outfile);
            _exit(1);
        }

        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
}

static int execute_pipeline(char *cwd, int cmds_num, CommandStage stages[])
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
            signal(SIGINT, SIG_DFL);

            if (i > 0)
                if (dup2(pipefds[(i - 1) * 2], STDIN_FILENO) < 0)
                {
                    perror("dup2 stdin");
                    _exit(1);
                }

            if (i < cmds_num - 1)
                if (dup2(pipefds[i * 2 + 1], STDOUT_FILENO) < 0)
                {
                    perror("dup2 stdout");
                    _exit(1);
                }

            for (int j = 0; j < 2 * (cmds_num - 1); j++)
                close(pipefds[j]);

            apply_redirections(&stages[i]);

            if (stages[i].argc == 0)
                _exit(0);

            if (strcmp(stages[i].argv[0], "pwd") == 0)
            {
                printf("%s\n", cwd);
                _exit(0);
            }

            if (strcmp(stages[i].argv[0], "cd") == 0)
                _exit(0);

            execv(stages[i].argv[0], stages[i].argv);
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

        CommandStage stages[MAX_TOKENS];
        memset(stages, 0, sizeof(stages));

        int cmds_num = 0;
        bool syntax_error = false;

        for (int i = 0; i < token_count; i++)
        {
            if (tokens[i].type == TOKEN_PIPE)
            {
                if (stages[cmds_num].argc == 0 && stages[cmds_num].infile == NULL && stages[cmds_num].outfile == NULL)
                {
                    fprintf(stderr, "Syntax error near unexpected token '|'!\n");
                    syntax_error = true;
                    break;
                }

                stages[cmds_num].argv[stages[cmds_num].argc] = NULL;
                cmds_num++;
            }
            else if (tokens[i].type == TOKEN_REDIRECT_IN)
            {
                if (i + 1 >= token_count || tokens[i + 1].type != TOKEN_WORD)
                {
                    fprintf(stderr, "Syntax error near unexpected token '<'!\n");
                    syntax_error = true;
                    break;
                }

                stages[cmds_num].infile = tokens[i + 1].value;
                i++;
            }
            else if (tokens[i].type == TOKEN_REDIRECT_OUT || tokens[i].type == TOKEN_REDIRECT_APPEND)
            {
                if (i + 1 >= token_count || tokens[i + 1].type != TOKEN_WORD)
                {
                    fprintf(stderr, "Syntax error near unexpected token '>'!\n");
                    syntax_error = true;
                    break;
                }

                stages[cmds_num].outfile = tokens[i + 1].value;
                stages[cmds_num].append_out = (tokens[i].type == TOKEN_REDIRECT_APPEND);
                i++;
            }
            else if (tokens[i].type == TOKEN_WORD)
                stages[cmds_num].argv[stages[cmds_num].argc++] = tokens[i].value;
        }

        if (syntax_error)
            continue;

        if (stages[cmds_num].argc > 0 || stages[cmds_num].infile != NULL || stages[cmds_num].outfile != NULL)
        {
            stages[cmds_num].argv[stages[cmds_num].argc] = NULL;
            cmds_num++;
        }

        if (cmds_num == 0)
            continue;

        if (cmds_num == 1 && stages[0].argc > 0)
        {
            if (strcmp(stages[0].argv[0], "exit") == 0)
            {
                int status = 0;
                if (stages[0].argc > 1)
                    status = atoi(stages[0].argv[1]);

                return status;
            }

            if (strcmp(stages[0].argv[0], "cd") == 0)
            {
                if (stages[0].argc < 2)
                {
                    fprintf(stderr, "cd: missing path argument\n");
                    continue;
                }

                if (chdir(stages[0].argv[1]) != 0)
                    perror("cd");

                continue;
            }
        }

        // If I ever decide to unfortuantly add a PATH thingy, this will become "outdated"
        char *exec = stages[0].argv[0];
        if (exec[0] != '.' && exec[0] != '/')
        {
            char original[strlen(exec) + 6];
            memcpy(original, exec, strlen(exec) + 1);

            // Original
            memcpy(exec + 5, original, strlen(original));
            exec[strlen(exec)] = '\0';

            // Assumed prefix
            memcpy(exec, "/bin/", 5);
        }

        execute_pipeline(cwd, cmds_num, stages);
    }

    return 0;
}
