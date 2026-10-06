#include "gnrmake.h"
#include <stdio.h>

int gnr_make(const char *cfgfile)
{
    FILE *cfgfp = fopen(cfgfile, "rb");
    if (!cfgfp)
    {
        fputs("Unable to open config XML file!\n", stderr);
        perror("fopen");
        return 1;
    }

    fclose(cfgfp);
    return 0;
}
