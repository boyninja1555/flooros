#include "gnr/gnrmake.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

typedef enum
{
    TOKEN_EOF,

    TOKEN_KEY,
    TOKEN_VALUE,
} TokenType;

typedef struct
{
    TokenType type;
    union
    {
        char *key;
        char *value;
    };
} Token;

typedef struct
{
    char *source;
    size_t line, col;

    struct
    {
        Token *data;
        size_t length;
        size_t cap;
    } tokens;
} LexerState;

Token token_copy(const Token token)
{
    Token t = {.type = token.type};
    switch (t.type)
    {
    case TOKEN_KEY:
    {
        t.key = malloc(strlen(token.key) + 1);
        strcpy(t.key, token.key);
        break;
    }

    case TOKEN_VALUE:
    {
        t.value = malloc(strlen(token.value) + 1);
        strcpy(t.value, token.value);
        break;
    }
    }

    return t;
}

void token_free(Token *token)
{
    switch (token->type)
    {
    case TOKEN_KEY:
    {
        free(token->key);
        break;
    }

    case TOKEN_VALUE:
    {
        free(token->value);
        break;
    }
    }

    token->type = 0;
}

Token *lexer_pushtoken(LexerState *lexer, const Token token)
{
    if (lexer->tokens.length >= lexer->tokens.cap)
    {
        lexer->tokens.cap *= 2;
        Token *tokens = realloc(lexer->tokens.data, sizeof(Token) * lexer->tokens.cap);
        if (!tokens)
        {
            lexer->tokens.cap /= 2;
            fputs("Failed to increase space for tokens during tokenization! Out of memory.\n", stderr);
            return NULL;
        }

        lexer->tokens.data = tokens;
    }

    lexer->tokens.data[lexer->tokens.length] = token_copy(token);
    return &lexer->tokens.data[lexer->tokens.length++];
}

void lexer_free(LexerState *lexer)
{
    for (size_t i = 0; i < lexer->tokens.length; i++)
        token_free(&lexer->tokens.data[i]);
    free(lexer->tokens.data);
    lexer->tokens.length = 0;
    lexer->tokens.cap = 0;
}

bool tokenize(LexerState *lexer)
{
    lexer->line = 1;
    lexer->col = 1;
    lexer->tokens.length = 0;
    lexer->tokens.cap = 8;
    lexer->tokens.data = malloc(sizeof(Token) * lexer->tokens.cap);

    size_t keylen = 0;
    size_t keycap = 1;
    char *key = malloc(keycap);
    key[keylen] = '\0';

    size_t srclen = strlen(lexer->source);
    for (size_t i = 0; i < srclen; i++)
    {
        char c = lexer->source[i];

        if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
            continue;

        if (c == '#')
        {
            while (i < srclen && c != '\n')
                c = lexer->source[++i];
            continue;
        }

        if (c == '=')
        {
            if (keylen == 0)
            {
                fprintf(stderr, "[%zu:%zu] Unexpected assignment prior to key definition!\n", lexer->line, lexer->col);
                return false;
            }

            size_t valuelen = 0;
            size_t valuecap = 1;
            char *value = malloc(valuecap);
            value[valuelen] = '\0';
            while (i < srclen)
            {
                c = lexer->source[i++];
                if (c == '\n')
                    break;

                if (valuelen + 1 >= valuecap)
                {
                    valuecap *= 2;
                    char *newvalue = realloc(value, valuecap);
                    if (!newvalue)
                    {
                        valuecap /= 2;
                        fputs("Failed to increase space for temporary value during tokenization! Out of memory.\n", stderr);
                        continue;
                    }

                    value = newvalue;
                }

                value[valuelen++] = c;
                value[valuelen] = '\0';
            }

            lexer_pushtoken(lexer, (Token){.type = TOKEN_KEY, .key = key});
            lexer_pushtoken(lexer, (Token){.type = TOKEN_VALUE, .value = value});
            free(value);
            keylen = 0;
            valuelen = 0;
            continue;
        }

        if (keylen + 1 >= keycap)
        {
            keycap *= 2;
            char *newkey = realloc(key, keycap);
            if (!newkey)
            {
                keycap /= 2;
                fputs("Failed to increase space for temporary key during tokenization! Out of memory.\n", stderr);
                continue;
            }

            key = newkey;
        }

        key[keylen++] = c;
        key[keylen] = '\0';
    }

    lexer_pushtoken(lexer, (Token){.type = TOKEN_EOF});
    free(key);
    return true;
}

int gnr_make(const char *cfgfile)
{
    FILE *cfgfp = fopen(cfgfile, "rb");
    if (!cfgfp)
    {
        fputs("Unable to open config file!\n", stderr);
        perror("fopen");
        return 1;
    }

    fseek(cfgfp, 0, SEEK_END);
    long cfglen = ftell(cfgfp);
    fseek(cfgfp, 0, SEEK_SET);

    LexerState lexer = {.source = malloc(cfglen + 1)};
    fread(lexer.source, 1, cfglen, cfgfp);
    fclose(cfgfp);
    lexer.source[cfglen] = '\0';

    if (!tokenize(&lexer))
    {
        free(lexer.source);
        lexer_free(&lexer);
        return 1;
    }

    free(lexer.source);

    for (size_t i = 0; i < lexer.tokens.length; i++)
    {
        Token *token = &lexer.tokens.data[i];
        switch (token->type)
        {
        case TOKEN_EOF:
        {
            puts("EOF");
            break;
        }

        case TOKEN_KEY:
        {
            printf("KEY -> %s\n", token->key);
            break;
        }

        case TOKEN_VALUE:
        {
            printf("VALUE -> %s\n", token->value);
            break;
        }

        default:
        {
            puts("NULL");
            break;
        }
        }
    }

    lexer_free(&lexer);
    return 0;
}
