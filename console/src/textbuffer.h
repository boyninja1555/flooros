#pragma once

#include <stdbool.h>
#include "typing.h"

#define TB_WIDTH 80
#define TB_HEIGHT 25
#define TB_SIZE (TB_WIDTH * TB_HEIGHT)

typedef struct
{
    char c;
    bool bold;
    bool italic;
    bool underline;
    u8 foreground;
    u8 background;
} TbCell;

void textbuffer_clear();

void textbuffer_set(int x, int y, TbCell cell);

TbCell *textbuffer_get();
