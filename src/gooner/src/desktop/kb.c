#include "desktop/kb.h"
#include <sys/ioctl.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

bool fd_is_keyboard(int fd)
{
    unsigned long ev_bits[1] = {0};
    unsigned long key_bits[KEY_BYTES_MAX / sizeof(unsigned long)] = {0};
    if (ioctl(fd, EVIOCGBIT(0, sizeof(ev_bits)), ev_bits) < 0)
        return false;
    if (!(ev_bits[0] & (1 << EV_KEY)))
        return false;
    if (ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(key_bits)), key_bits) < 0)
        return false;

    int bit_index = KEY_A / (8 * sizeof(unsigned long));
    int bit_shift = KEY_A % (8 * sizeof(unsigned long));
    return key_bits[bit_index] & (1UL << bit_shift);
}

bool fd_get_keyboard(char device_path[DEVICE_PATH_MAX])
{
    for (int i = 0; i < 32; i++)
    {
        snprintf(device_path, DEVICE_PATH_MAX, "/dev/input/event%d", i);
        int fd = open(device_path, O_RDONLY | O_NONBLOCK);
        if (fd != -1)
        {
            if (fd_is_keyboard(fd))
            {
                close(fd);
                return true;
            }

            close(fd);
        }
    }

    return false;
}
