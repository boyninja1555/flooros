#include "renderer.h"
#include "framebuffer.h"
#include "textbuffer.h"
#include "font.h"

static void draw_pixel(int x, int y, u32 color)
{
    u32 *pixels = (u32 *)framebuffer_get();
    pixels[y * framebuffer_width() + x] = color;
}

static void draw_character(int x, int y, char c, u32 fg, u32 bg)
{
    const u8 *glyph = font[(u8)c];
    for (int row = 0; row < FONT_HEIGHT; row++)
        for (int col = 0; col < FONT_WIDTH; col++)
        {
            bool pixel = glyph[row] & (1 << (7 - col));
            draw_pixel(x + col, y + row, pixel ? fg : bg);
        }
}

void renderer_draw()
{
    TbCell *cells = textbuffer_get();
    for (int y = 0; y < TB_HEIGHT; y++)
        for (int x = 0; x < TB_WIDTH; x++)
            draw_character(x * FONT_WIDTH, y * FONT_HEIGHT, cells[y * TB_WIDTH + x].c, 0xffffffff, 0xff000000);
}
