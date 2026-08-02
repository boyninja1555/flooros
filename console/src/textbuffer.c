#include "textbuffer.h"

static TbCell buffer[TB_SIZE];

void textbuffer_clear()
{
    for (int i = 0; i < TB_SIZE; i++)
        buffer[i] = (TbCell){
            .c = ' ',
            .bold = false,
            .italic = false,
            .underline = false,
            .foreground = 7,
            .background = 0,
        };
}

void textbuffer_set(int x, int y, TbCell cell)
{
    if (x < 0 || y < 0)
        return;

    if (x >= TB_WIDTH || y >= TB_HEIGHT)
        return;

    buffer[y * TB_WIDTH + x] = cell;
}

TbCell *textbuffer_get()
{
    return buffer;
}
