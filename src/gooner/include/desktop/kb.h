#pragma once

#define DEVICE_PATH_MAX 19
#define KEY_BYTES_MAX ((KEY_MAX + 7) / 8)

#include <stdbool.h>

bool fd_is_keyboard(int fd);

bool fd_get_keyboard(char device_path[DEVICE_PATH_MAX]);
