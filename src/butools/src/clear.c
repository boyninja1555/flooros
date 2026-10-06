#include <unistd.h>
#include <string.h>

int main(void)
{
    const char *clear = "\033[H\033[2J";
    write(STDOUT_FILENO, clear, strlen(clear));
    return 0;
}
