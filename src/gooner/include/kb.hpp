#pragma once

#define DEVICE_PATH_MAX 19
#define KEY_BYTES_MAX ((KEY_MAX + 7) / 8)

namespace Keyboard
{
    bool is(int fd);

    bool get(char devicepath_ptr[DEVICE_PATH_MAX]);
};
