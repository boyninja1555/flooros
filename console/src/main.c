#include <unistd.h>
#include <stdio.h>
#include "terminal.h"
#include "ansi.h"
#include "framebuffer.h"
#include "renderer.h"

int main()
{
    framebuffer_init();
    terminal_init();

    // char c;
    // while (read(0, &c, 1) > 0)
    // {
    //     AnsiToken token;
    //     if (ansi_next(c, &token))
    //         if (token.type == ANSI_TEXT)
    //             terminal_write(token.data[0]);
    //     renderer_draw();
    // }

    terminal_write('A');
    terminal_write(' ');
    for (int i = 1; i < 3; i++)
        terminal_write('A');
    for (int i = 1; i < 2; i++)
        terminal_write(' ');
    for (int i = 1; i < 2; i++)
        terminal_write('A');
    terminal_write(' ');
    terminal_write('A');

    while (1)
    {
        renderer_draw();
    }

    return 0;
}
