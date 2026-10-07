#include "desktop/fb.h"
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <string.h>

static Framebuf fb;
static int fd_tty;

uint32_t framebuf_px(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue)
{
    uint32_t pixel = 0;
    pixel |= ((uint32_t)red >> (8 - fb.red.length)) << fb.red.offset;
    pixel |= ((uint32_t)green >> (8 - fb.green.length)) << fb.green.offset;
    pixel |= ((uint32_t)blue >> (8 - fb.blue.length)) << fb.blue.offset;
    if (fb.transp.length)
        pixel |= ((uint32_t)alpha >> (8 - fb.transp.length)) << fb.transp.offset;
    return pixel;
}

int framebuf_init(void)
{
    struct fb_fix_screeninfo fix;
    struct fb_var_screeninfo var;

    fb.fd = open("/dev/fb0", O_RDWR);
    if (fb.fd < 0)
        return -1;
    if (ioctl(fb.fd, FBIOGET_FSCREENINFO, &fix) < 0)
        return -1;
    if (ioctl(fb.fd, FBIOGET_VSCREENINFO, &var) < 0)
        return -1;

    fb.width = var.xres;
    fb.height = var.yres;
    fb.pitch = fix.line_length;
    fb.bpp = var.bits_per_pixel;
    fb.size = fix.smem_len;

    fb.red = var.red;
    fb.green = var.green;
    fb.blue = var.blue;
    fb.transp = var.transp;

    fb.memory = mmap(NULL, fb.size, PROT_READ | PROT_WRITE, MAP_SHARED, fb.fd, 0);
    if (fb.memory == MAP_FAILED)
        return -1;

    fd_tty = open("/dev/console", O_RDWR);
    ioctl(fd_tty, KDSETMODE, KD_GRAPHICS);
    return 0;
}

void framebuf_getsize(uint32_t *w, uint32_t *h)
{
    *w = fb.width;
    *h = fb.height;
}

void framebuf_pxclear(uint32_t color)
{
    for (uint32_t y = 0; y < fb.height; y++)
        for (uint32_t x = 0; x < fb.width; x++)
            framebuf_putpx(x, y, color);
}

void framebuf_pxfill(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
    for (uint32_t row = x; row < x + w; row++)
        for (uint32_t col = y; col < y + h; col++)
            framebuf_putpx(row, col, color);
}

void framebuf_pxtext(uint32_t x, uint32_t y, uint32_t color, const char *text)
{
    size_t text_length = strlen(text);
    for (size_t i = 0; i < text_length; i++)
        framebuf_pxchar(x + i * FONTC_WIDTH, y, color, text[i]);
}

void framebuf_pxchar(uint32_t x, uint32_t y, uint32_t color, char c)
{
    uint8_t glyph[FONTC_HEIGHT] = {0};
    switch (c)
    {
    case ' ':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b00000000,
                          0b00000000,
                          0b00000000,
                          0b00000000,
                          0b00000000,
                          0b00000000,
                          0b00000000,
                          0b00000000,
                      },
               FONTC_HEIGHT);
        break;
    case 'A':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b00011000,
                          0b00111100,
                          0b00100100,
                          0b01100110,
                          0b01000010,
                          0b01111110,
                          0b01000010,
                          0b01000010,
                      },
               FONTC_HEIGHT);
        break;
    case 'B':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b01111100,
                          0b01000010,
                          0b01000010,
                          0b01111100,
                          0b01000010,
                          0b01000010,
                          0b01000010,
                          0b01111100,
                      },
               FONTC_HEIGHT);
        break;
    case 'C':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b00111100,
                          0b01100110,
                          0b01000010,
                          0b01000000,
                          0b01000000,
                          0b01000010,
                          0b01100110,
                          0b00111100,
                      },
               FONTC_HEIGHT);
        break;
    case 'D':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b01111000,
                          0b01001100,
                          0b01000110,
                          0b01000010,
                          0b01000010,
                          0b01000110,
                          0b01001100,
                          0b01111000,
                      },
               FONTC_HEIGHT);
        break;
    case 'E':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b01111110,
                          0b01000000,
                          0b01000000,
                          0b01111110,
                          0b01000000,
                          0b01000000,
                          0b01000000,
                          0b01111110,
                      },
               FONTC_HEIGHT);
        break;
    case 'F':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b01111110,
                          0b01000000,
                          0b01000000,
                          0b01111110,
                          0b01000000,
                          0b01000000,
                          0b01000000,
                          0b01000000,
                      },
               FONTC_HEIGHT);
        break;
    case 'G':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b00111100,
                          0b01100110,
                          0b01000000,
                          0b01001110,
                          0b01000010,
                          0b01000010,
                          0b01100110,
                          0b00111100,
                      },
               FONTC_HEIGHT);
        break;
    case 'H':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b01000010,
                          0b01000010,
                          0b01000010,
                          0b01111110,
                          0b01000010,
                          0b01000010,
                          0b01000010,
                          0b01000010,
                      },
               FONTC_HEIGHT);
        break;
    case 'I':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b01111110,
                          0b00010000,
                          0b00010000,
                          0b00010000,
                          0b00010000,
                          0b00010000,
                          0b00010000,
                          0b01111110,
                      },
               FONTC_HEIGHT);
        break;
    case 'J':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b01111110,
                          0b00000100,
                          0b00000100,
                          0b00000100,
                          0b00000100,
                          0b00000100,
                          0b01101100,
                          0b00111000,
                      },
               FONTC_HEIGHT);
        break;
    case 'K':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b01000110,
                          0b01001100,
                          0b01011000,
                          0b01110000,
                          0b01011000,
                          0b01001100,
                          0b01000110,
                          0b01000110,
                      },
               FONTC_HEIGHT);
        break;
    case 'L':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b01000000,
                          0b01000000,
                          0b01000000,
                          0b01000000,
                          0b01000000,
                          0b01000000,
                          0b01000000,
                          0b01111110,
                      },
               FONTC_HEIGHT);
        break;
    case 'M':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b01101100,
                          0b01111100,
                          0b01010100,
                          0b01010100,
                          0b01010100,
                          0b01010100,
                          0b01010100,
                          0b01010100,
                      },
               FONTC_HEIGHT);
        break;
    case 'N':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b01100010,
                          0b01110010,
                          0b01010010,
                          0b01011010,
                          0b01001010,
                          0b01001010,
                          0b01001110,
                          0b01000110,
                      },
               FONTC_HEIGHT);
        break;
    case 'O':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b00111100,
                          0b01100110,
                          0b01000010,
                          0b01000010,
                          0b01000010,
                          0b01000010,
                          0b01100110,
                          0b00111100,
                      },
               FONTC_HEIGHT);
        break;
    case 'P':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b01111100,
                          0b01000110,
                          0b01000010,
                          0b01000110,
                          0b01111100,
                          0b01000000,
                          0b01000000,
                          0b01000000,
                      },
               FONTC_HEIGHT);
        break;
    case 'Q':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b00111100,
                          0b01100110,
                          0b01000010,
                          0b01000010,
                          0b01000010,
                          0b01100110,
                          0b00111111,
                          0b00000011,
                      },
               FONTC_HEIGHT);
        break;
    case 'R':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b01111100,
                          0b01000110,
                          0b01000010,
                          0b01000110,
                          0b01111100,
                          0b01110000,
                          0b01011100,
                          0b01000110,
                      },
               FONTC_HEIGHT);
        break;
    case 'S':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b00111100,
                          0b01100110,
                          0b01000000,
                          0b00111100,
                          0b00000110,
                          0b00000010,
                          0b01100110,
                          0b00111100,
                      },
               FONTC_HEIGHT);
        break;
    case 'T':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b01111110,
                          0b00010000,
                          0b00010000,
                          0b00010000,
                          0b00010000,
                          0b00010000,
                          0b00010000,
                          0b00010000,
                      },
               FONTC_HEIGHT);
        break;
    case 'U':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b01000010,
                          0b01000010,
                          0b01000010,
                          0b01000010,
                          0b01000010,
                          0b01000010,
                          0b01100110,
                          0b00111100,
                      },
               FONTC_HEIGHT);
        break;
    case 'V':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b01000010,
                          0b01000010,
                          0b01000010,
                          0b00100100,
                          0b00100100,
                          0b00100100,
                          0b00011000,
                          0b00011000,
                      },
               FONTC_HEIGHT);
        break;
    case 'W':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b01010100,
                          0b01010100,
                          0b01010100,
                          0b01010100,
                          0b01010100,
                          0b01010100,
                          0b01010100,
                          0b01101100,
                      },
               FONTC_HEIGHT);
        break;
    case 'X':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b01000010,
                          0b01000010,
                          0b00100100,
                          0b00011000,
                          0b00011000,
                          0b00100100,
                          0b01000010,
                          0b01000010,
                      },
               FONTC_HEIGHT);
        break;
    case 'Y':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b01000100,
                          0b01000100,
                          0b00101000,
                          0b00101000,
                          0b00101000,
                          0b00010000,
                          0b00010000,
                          0b00010000,
                      },
               FONTC_HEIGHT);
        break;
    case 'Z':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b01111110,
                          0b00000010,
                          0b00000110,
                          0b00001100,
                          0b00110000,
                          0b01100000,
                          0b01000000,
                          0b01111110,
                      },
               FONTC_HEIGHT);
        break;
    case ',':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b00000000,
                          0b00000000,
                          0b00000000,
                          0b00000000,
                          0b00000000,
                          0b00001000,
                          0b00001000,
                          0b00010000,
                      },
               FONTC_HEIGHT);
        break;
    case '.':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b00000000,
                          0b00000000,
                          0b00000000,
                          0b00000000,
                          0b00000000,
                          0b00000000,
                          0b00010000,
                          0b00010000,
                      },
               FONTC_HEIGHT);
        break;
    case '!':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b00010000,
                          0b00010000,
                          0b00010000,
                          0b00010000,
                          0b00010000,
                          0b00000000,
                          0b00010000,
                          0b00010000,
                      },
               FONTC_HEIGHT);
        break;
    case '?':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b00111100,
                          0b01100110,
                          0b00000110,
                          0b00001100,
                          0b00011000,
                          0b00000000,
                          0b00010000,
                          0b00010000,
                      },
               FONTC_HEIGHT);
        break;
    case ';':
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b00001000,
                          0b00001000,
                          0b00000000,
                          0b00000000,
                          0b00000000,
                          0b00001000,
                          0b00001000,
                          0b00010000,
                      },
               FONTC_HEIGHT);
        break;
    default:
        memcpy(glyph, (uint8_t[FONTC_HEIGHT]){
                          0b11111111,
                          0b11111111,
                          0b11111111,
                          0b11111111,
                          0b11111111,
                          0b11111111,
                          0b11111111,
                          0b11111111,
                      },
               FONTC_HEIGHT);
        break;
    }

    framebuf_pxglyph(x, y, color, glyph);
}

void framebuf_pxglyph(uint32_t x, uint32_t y, uint32_t color, uint8_t glyph[FONTC_HEIGHT])
{
    for (uint32_t row = 0; row < FONTC_HEIGHT; row++)
        for (uint32_t col = 0; col < FONTC_WIDTH; col++)
            if (glyph[row] & (0x80 >> col))
                framebuf_putpx(x + col, y + row, color);
}

void framebuf_putpx(uint32_t x, uint32_t y, uint32_t color)
{
    if (x >= fb.width || y >= fb.height)
        return;
    uint32_t offset = y * fb.pitch + x * 4;
    fb.memory[offset + 0] = color;
    fb.memory[offset + 1] = color >> 8;
    fb.memory[offset + 2] = color >> 16;
    fb.memory[offset + 3] = color >> 24;
}

void framebuf_close(void)
{
    ioctl(fd_tty, KDSETMODE, KD_TEXT);
    close(fd_tty);

    munmap(fb.memory, fb.size);
    close(fb.fd);
}
