#include <sys/reboot.h>
#include <stdio.h>

int main()
{
    printf("Shutting down...\n");
    reboot(RB_POWER_OFF);
    return 0;
}
