#include <sys/mount.h>
#include <sys/wait.h>
#include <sys/reboot.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

static void mount_fs(const char *source, const char *target, const char *type)
{
    if (mount(source, target, type, 0, NULL) < 0)
        perror(target);
}

static void setup_filesystem()
{
    printf("Initializing filesystem...\n");
    mount_fs("proc", "/proc", "proc");
    mount_fs("sysfs", "/sys", "sysfs");
    mount_fs("devtmpfs", "/dev", "devtmpfs");
    mount_fs("devpts", "/dev/pts", "devpts");
    mount_fs("tmpfs", "/tmp", "tmpfs");
    printf("Filesystem initialized!\n");
}

// static int start_console()
// {
//     pid_t pid = fork();
//
//     if (pid == 0)
//     {
//         char *args[] = {"/bin/console", NULL};
//         execv(args[0], args);
//         perror("console");
//         _exit(1);
//     }
//
//     if (pid < 0)
//     {
//         perror("console fork");
//         return -1;
//     }
//
//     return 0;
// }

static int attach_console()
{
    int console = open("/dev/console", O_RDWR);
    if (console < 0)
    {
        perror("open console");
        return -1;
    }

    dup2(console, STDIN_FILENO);
    dup2(console, STDOUT_FILENO);
    dup2(console, STDERR_FILENO);
    if (console > STDERR_FILENO)
        close(console);

    return 0;
}

static void spawn_shell()
{
    while (1)
    {
        pid_t pid = fork();

        if (pid == 0)
        {
            char *args[] = {"/bin/sf", NULL};
            execv(args[0], args);
            perror("shell");
            _exit(1);
        }

        if (pid < 0)
        {
            perror("shell fork");
            sleep(1);
            continue;
        }

        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid");
            continue;
        }

        if (WIFEXITED(status))
            printf("shellyfloor exited with status %d\n", WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            printf("shellyfloor killed by signal %d\n", WTERMSIG(status));

        printf("Restarting shellyfloor...\n");
    }
}

int main()
{
    setup_filesystem();
    // start_console();
    sleep(1);
    attach_console();
    // printf("\033[2J");
    // printf("\033[H");
    printf("\033[1;31mFloorOS x Shellyfloor (sf)\033[0m\n");
    spawn_shell();
    printf("System halted!\n");
    reboot(RB_POWER_OFF);
    return 0;
}
