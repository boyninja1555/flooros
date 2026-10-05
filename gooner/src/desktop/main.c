#define _POSIX_C_SOURCE 200112L
#include <sys/signal.h>
#include <time.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "desktop/main.h"
#include "desktop/fb.h"
#include "desktop/image.h"

#define LOGO_PADDING 8
#define TEXT_TOP "FloorOS"
#define TEXT_BOT "Gooner Desktop"

static bool running = true;

static uint8_t *logo = NULL;
static uint32_t logo_x = 0, logo_y = 0, logo_width = 0, logo_height = 0;

static uint32_t text_top_x = 0, text_top_y = 0, text_top_width = 0;
static uint32_t text_bot_x = 0, text_bot_y = 0, text_bot_width = 0;

void desktop_render(void)
{
    framebuf_pxclear(framebuf_px(0xFF, 0, 0x7F, 0xFF));

    // Text
    framebuf_pxtext(text_top_x, text_top_y, framebuf_px(0xFF, 0x40, 0x40, 0x40), TEXT_TOP);
    framebuf_pxtext(text_bot_x, text_bot_y, framebuf_px(0xFF, 0x20, 0x20, 0x20), TEXT_BOT);

    // Logo
    framebuf_pxfill(logo_x - LOGO_PADDING, logo_y - LOGO_PADDING, logo_width + (LOGO_PADDING * 2), logo_height + (LOGO_PADDING * 2), framebuf_px(0xFF, 0x7F, 0x7F, 0x7F));
    if (logo)
        for (uint32_t y = 0; y < logo_height; y++)
            for (uint32_t x = 0; x < logo_width; x++)
            {
                uint32_t i = (y * logo_width + x) * 3;
                framebuf_putpx(logo_x + x, logo_y + y, framebuf_px(0xFF, logo[i], logo[i + 1], logo[i + 2]));
            }
}

int main_desktop(int argc, const char *argv[])
{
    imgld_ppm("/etc/logo.ppm", &logo, &logo_width, &logo_height);

    if (framebuf_init() < 0)
        return 1;
    uint32_t fb_w, fb_h;
    framebuf_getsize(&fb_w, &fb_h);

    logo_x = fb_w / 2 - logo_width / 2;
    logo_y = fb_h / 2 - logo_height / 2 - logo_height;

    text_top_width = FONTC_WIDTH * strlen(TEXT_TOP);
    text_bot_width = FONTC_WIDTH * strlen(TEXT_BOT);
    text_top_x = fb_w / 2 - text_top_width / 2;
    text_bot_x = fb_w / 2 - text_bot_width / 2;
    text_top_y = fb_h / 2 - logo_height / 2 + FONTC_HEIGHT;
    text_bot_y = fb_h / 2 - logo_height / 2 + FONTC_HEIGHT * 2;

    desktop_render();

    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);
    while (running)
    {
        // desktop_render();

        next.tv_nsec += 16666667;
        while (next.tv_nsec >= 1000000000)
        {
            next.tv_sec++;
            next.tv_nsec -= 1000000000;
        }

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }

    framebuf_close();
    return 0;
}
