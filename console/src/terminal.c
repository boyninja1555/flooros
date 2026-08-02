#include "terminal.h"
#include "textbuffer.h"

static int cursor_x;
static int cursor_y;
static u8 foreground;
static u8 background;

void terminal_init()
{
    cursor_x = 0;
    cursor_y = 0;
    foreground = 7;
    background = 0;
    textbuffer_clear();
}

void terminal_write(char c)
{
    if (c == '\n')
    {
        cursor_x = 0;
        cursor_y++;
        return;
    }

    TbCell cell = {.c = c, .foreground = foreground, .background = background};
    textbuffer_set(cursor_x, cursor_y, cell);
    cursor_x++;
    if (cursor_x >= TB_WIDTH)
    {
        cursor_x = 0;
        cursor_y++;
    }
}
