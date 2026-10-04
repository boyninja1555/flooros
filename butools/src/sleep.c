#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Missing seconds!\n\t%s <seconds>\n", argv[0]);
        return 1;
    }

    sleep(atoi(argv[1]));
    return 0;
}
