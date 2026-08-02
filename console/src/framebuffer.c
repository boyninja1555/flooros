#include "framebuffer.h"
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

static int fb_fd = -1;
static u8 *fb = NULL;
static struct fb_var_screeninfo screen;

bool framebuffer_init()
{
    fb_fd = open("/dev/fb0", O_RDWR);

    if (fb_fd < 0)
    {
        perror("open /dev/fb0");
        return false;
    }

    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &screen) < 0)
    {
        perror("FBIOGET_VSCREENINFO");
        return false;
    }

    size_t size = screen.xres * screen.yres * screen.bits_per_pixel / 8;
    fb = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if (fb == MAP_FAILED)
    {
        perror("mmap framebuffer");
        fb = NULL;
        return false;
    }

    return true;
}

void framebuffer_clear(u32 color)
{
    if (!fb)
        return;

    u32 *pixels = (u32 *)fb;
    int count = screen.xres * screen.yres;
    for (int i = 0; i < count; i++)
        pixels[i] = color;
}

u8 *framebuffer_get()
{
    return fb;
}

int framebuffer_width()
{
    return screen.xres;
}

int framebuffer_height()
{
    return screen.yres;
}

int framebuffer_bpp()
{
    return screen.bits_per_pixel;
}
