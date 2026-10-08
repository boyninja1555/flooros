#include "image.hpp"
#include <cstring>
#include <iostream>

bool ImageLoad::ppm(const char *filename, UI::color_t **data_ptr, uint32_t *w, uint32_t *h)
{
    FILE *file = std::fopen(filename, "rb");
    if (!file)
    {
        std::cerr << "Unable to open a PPM image!" << std::endl;
        std::perror("fopen");
        return false;
    }

    char magic[3];
    int max;
    std::fscanf(file, "%2s\n%d %d\n%d\n", magic, w, h, &max);

    int size = (*w) * (*h);
    std::uint8_t *data_rgb = static_cast<std::uint8_t *>(std::malloc(size * 3));
    std::fread(data_rgb, 3, size, file);
    std::fclose(file);
    *data_ptr = static_cast<UI::color_t *>(std::malloc(size * sizeof(UI::color_t)));
    for (std::size_t i = 0; i < size; i++)
    {
        std::uint8_t r = data_rgb[i * 3];
        std::uint8_t g = data_rgb[i * 3 + 1];
        std::uint8_t b = data_rgb[i * 3 + 2];
        (*data_ptr)[i] = (static_cast<UI::color_t>(0xFF) << 24) | (static_cast<UI::color_t>(r) << 16) | (static_cast<UI::color_t>(g) << 8) | static_cast<UI::color_t>(b);
    }

    std::free(data_rgb);
    return true;
}
