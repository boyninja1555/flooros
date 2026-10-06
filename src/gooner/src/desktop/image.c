#include "desktop/image.h"
#include <stdlib.h>
#include <stdio.h>

bool imgld_ppm(const char *filename, uint8_t **pixels, uint32_t *width, uint32_t *height)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        fputs("Unable to open a PPM image!\n", stderr);
        perror("fopen");
        return false;
    }

    char magic[3];
    int max_value;
    fscanf(file, "%2s\n%d %d\n%d\n", magic, width, height, &max_value);

    int size = (*width) * (*height) * 3;
    *pixels = (uint8_t *)malloc(size);
    fread(*pixels, 1, size, file);
    fclose(file);
    return true;
}
