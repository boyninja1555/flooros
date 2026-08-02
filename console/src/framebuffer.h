#pragma once

#include <stdbool.h>
#include "typing.h"

bool framebuffer_init();

void framebuffer_clear(u32 color);

u8 *framebuffer_get();

int framebuffer_width();
int framebuffer_height();
int framebuffer_bpp();
