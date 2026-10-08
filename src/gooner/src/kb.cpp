#include "kb.hpp"
#include <sys/ioctl.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

bool Keyboard::is(int fd)
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

bool Keyboard::get(char devicepath_ptr[DEVICE_PATH_MAX])
{
    for (int i = 0; i < 32; i++)
    {
        snprintf(devicepath_ptr, DEVICE_PATH_MAX, "/dev/input/event%d", i);
        int fd = open(devicepath_ptr, O_RDONLY | O_NONBLOCK);
        if (fd != -1)
        {
            if (is(fd))
            {
                close(fd);
                return true;
            }

            close(fd);
        }
    }

    return false;
}

bool Mouse::is(int fd)
{
    unsigned long ev_bits[1] = {0};
    unsigned long rel_bits[REL_MAX / (8 * sizeof(unsigned long)) + 1] = {0};
    if (ioctl(fd, EVIOCGBIT(0, sizeof(ev_bits)), ev_bits) < 0)
        return false;
    if (!(ev_bits[0] & (1 << EV_REL)))
        return false;
    if (ioctl(fd, EVIOCGBIT(EV_REL, sizeof(rel_bits)), rel_bits) < 0)
        return false;

    int x_bit_index = REL_X / (8 * sizeof(unsigned long));
    int x_bit_shift = REL_X % (8 * sizeof(unsigned long));
    int y_bit_index = REL_Y / (8 * sizeof(unsigned long));
    int y_bit_shift = REL_Y % (8 * sizeof(unsigned long));
    return (rel_bits[x_bit_index] & (1UL << x_bit_shift)) && (rel_bits[y_bit_index] & (1UL << y_bit_shift));
}

bool Mouse::get(char devicepath_ptr[DEVICE_PATH_MAX])
{
    for (int i = 0; i < 32; i++)
    {
        snprintf(devicepath_ptr, DEVICE_PATH_MAX, "/dev/input/event%d", i);
        int fd = open(devicepath_ptr, O_RDONLY | O_NONBLOCK);
        if (fd != -1)
        {
            if (is(fd))
            {
                close(fd);
                return true;
            }

            close(fd);
        }
    }

    return false;
}
