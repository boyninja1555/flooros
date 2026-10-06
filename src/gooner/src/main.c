#include "gnr.h"
#include "desktop/main.h"

int main(int argc, const char *argv[])
{
    if (argc < 2)
        return main_desktop(argc, argv);
    return main_gnr(argc, argv);
}
