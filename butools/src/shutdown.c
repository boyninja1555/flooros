#include <sys/reboot.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
    printf("Shutting down...\n");
    sync();
    reboot(RB_POWER_OFF);
    return 0;
}
