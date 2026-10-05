#pragma once

#include <linux/fb.h>
#include <stdint.h>

#define FONTC_WIDTH 8
#define FONTC_HEIGHT 16

typedef struct
{
    int fd;
    uint8_t *memory;

    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;
    uint32_t size;

    struct fb_bitfield red;
    struct fb_bitfield green;
    struct fb_bitfield blue;
    struct fb_bitfield transp;
} Framebuf;

uint32_t framebuf_px(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue);

int framebuf_init(void);

void framebuf_getsize(uint32_t *w, uint32_t *h);

void framebuf_pxclear(uint32_t color);

void framebuf_pxfill(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);

void framebuf_pxtext(uint32_t x, uint32_t y, uint32_t color, const char *text);

void framebuf_pxchar(uint32_t x, uint32_t y, uint32_t color, char c);

void framebuf_pxglyph(uint32_t x, uint32_t y, uint32_t color, uint8_t glyph[FONTC_HEIGHT]);

void framebuf_putpx(uint32_t x, uint32_t y, uint32_t color);

void framebuf_close(void);
