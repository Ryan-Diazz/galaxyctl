#include <stdio.h>
#include "commands/commands.h"
#include "commands/sysfs.h"

int main(int argc, char *argv[])
{
    sysfs_set_command(argc, argv);

    if (argc < 2) {
        commands_help();
        return 0;
    }

    return commands_dispatch(argc - 1, argv + 1);
}