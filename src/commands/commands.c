#include <stdio.h>
#include <string.h>

#include "commands.h"
#include "recording/recording.h"
#include "powerlid/powerlid.h"
#include "profile/profile.h"

int commands_dispatch(int argc, char *argv[]) {
    const char *action = argv[0];

    if (strcmp(argv[0], "recording") == 0)
        return recording_command(argc - 1, argv + 1);

    if (strcmp(argv[0], "powerlid") == 0)
        return powerlid_command(argc - 1, argv + 1);

    if (strcmp(argv[0], "profile") == 0)
        return profile_command(argc - 1, argv + 1);

    // TODO: enable once implemented
    // if (strcmp(argv[0], "battery") == 0)
    //     return battery_command(argc - 1, argv + 1);
    //
    // if (strcmp(argv[0], "kb-light") == 0)
    //     return keyboard_command(argc - 1, argv + 1);

    if (strcmp(action, "help") == 0 || strcmp(action, "--help") == 0 || strcmp(action, "-h") == 0) {
        commands_help();
        return 0;
    }

    fprintf(stderr, "Unknown command: %s\n\n", argv[0]);
    commands_help();

    return 1;
}

void commands_help() {
    printf("Usage: galaxyctl <command> [action]\n\n");
    printf("Commands:\n");
    printf("    recording    Manage 'Block Recording' (webcam)\n");
    printf("    powerlid     Manage 'Power On Lid Open'\n");
    printf("    profile      Manage the 'Power Profile'\n");
    printf("\n");
    printf("Run 'galaxyctl <command> help' for the actions of a command.\n");
}