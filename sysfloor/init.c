#include <sys/mount.h>
#include <sys/wait.h>
#include <sys/reboot.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void mount_fs(const char *source, const char *target, const char *type)
{
    if (mount(source, target, type, 0, NULL) < 0)
        perror(target);
}

void spawn_shell()
{
    pid_t pid = fork();
    if (pid == 0)
    {
        char *args[] = {"/usr/bin/sh", NULL};
        execv("/usr/bin/sh", args);
        perror("exec shell");
        exit(1);
    }

    while (1)
    {
        int status;
        waitpid(-1, &status, 0);
        if (pid == -1)
            break;

        printf("process exited\n");
        if (WIFEXITED(status))
            spawn_shell();
    }
}

int main()
{
    printf("Booting FloorOS...\n");
    mount_fs("proc", "/proc", "proc");
    mount_fs("sysfs", "/sys", "sysfs");
    mount_fs("devtmpfs", "/dev", "devtmpfs");
    printf("Filesystem initialized!\n");
    spawn_shell();
    reboot(RB_POWER_OFF);
    return 0;
}
