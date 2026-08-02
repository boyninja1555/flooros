#pragma once

#include <stdbool.h>
#include "typing.h"

typedef enum
{
    ANSI_TEXT,
    ANSI_CSI,
} AnsiTokenType;

typedef struct
{
    AnsiTokenType type;
    char data[32];
    int length;
} AnsiToken;

bool ansi_next(char input, AnsiToken *token);
