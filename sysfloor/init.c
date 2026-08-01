#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void shutdown_handler(int sig)
{
    printf("\nShutting down sysfloor...\n");
    _exit(0);
}

int main()
{
    signal(SIGTERM, shutdown_handler);
    signal(SIGINT, shutdown_handler);
    printf("Sysfloor Shell\n");

    while (1)
    {
        printf("sysfloor$ ");
        fflush(stdout);

        char command[128];
        if (fgets(command, sizeof(command), stdin))
        {
            printf("You said something! %s", command);
        }
    }
}
