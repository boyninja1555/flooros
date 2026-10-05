#pragma once

#include <stdint.h>
#include <stdbool.h>

bool imgld_ppm(const char *filename, uint8_t **pixels, uint32_t *width, uint32_t *height);
