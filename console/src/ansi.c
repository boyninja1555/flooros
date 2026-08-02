#include "ansi.h"

static char buffer[32];
static int index = 0;
static bool escape = false;

bool ansi_next(char input, AnsiToken *token)
{
    if (input == '\033')
    {
        escape = true;
        index = 0;
        return false;
    }

    if (escape)
    {
        buffer[index++] = input;

        if (input == 'm')
        {
            token->type = ANSI_CSI;
            token->length = index;
            for (int i = 0; i < index; i++)
                token->data[i] = buffer[i];

            escape = false;
            return true;
        }

        return false;
    }

    token->type = ANSI_TEXT;
    token->data[0] = input;
    token->length = 1;
    return true;
}
