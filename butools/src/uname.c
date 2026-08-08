#include <sys/utsname.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    struct utsname buf;
    if (uname(&buf) != 0)
    {
        perror("uname");
        return 1;
    }

    bool show_sysname = false;
    bool show_nodename = false;
    bool show_release = false;
    bool show_machine = false;
    if (argc == 1)
    {
        show_sysname = true;
        show_nodename = true;
        show_release = true;
        show_machine = true;
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            if (argv[i][0] == '-')
            {
                for (size_t j = 1; j < strlen(argv[i]); j++)
                {
                    switch (argv[i][j])
                    {
                    case 'a':
                        show_sysname = true;
                        show_nodename = true;
                        show_release = true;
                        show_machine = true;
                        break;
                    case 's':
                        show_sysname = true;
                        break;
                    case 'n':
                        show_nodename = true;
                        break;
                    case 'r':
                        show_release = true;
                        break;
                    case 'm':
                        show_machine = true;
                        break;
                    default:
                        fprintf(stderr, "Invalid option! -- '%c'\n", argv[i][j]);
                        fprintf(stderr, "Usage: %s [-asnrvm]\n", argv[0]);
                        return 1;
                    }
                }
            }
            else
            {
                fprintf(stderr, "uname: unexpected argument '%s'\n", argv[i]);
                return 1;
            }
        }
    }

    bool need_space = false;

    if (show_sysname)
    {
        printf("%s", buf.sysname);
        need_space = true;
    }

    if (show_nodename)
    {
        if (need_space)
            printf(" ");

        printf("%s", buf.nodename);
        need_space = true;
    }

    if (show_release)
    {
        if (need_space)
            printf(" ");

        printf("%s", buf.release);
        need_space = true;
    }

    if (show_machine)
    {
        if (need_space)
            printf(" ");

        printf("%s", buf.machine);
    }

    printf("\n");
    return 0;
}
