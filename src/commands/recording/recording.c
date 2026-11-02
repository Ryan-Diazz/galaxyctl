#include <stdio.h>
#include <string.h>

#include "recording.h"
#include "sysfs.h"

#define BLOCK_RECORDING_PATH "/sys/class/firmware-attributes/samsung-galaxybook/attributes/block_recording/current_value"

// ====================================================
// Helpers
// ====================================================
static int recording_get(int *blocked) {
    long value;
    int err = sysfs_read_long(BLOCK_RECORDING_PATH, &value);
    if (err) {
        sysfs_error(BLOCK_RECORDING_PATH, err);
        return -1;
    }

    *blocked = value != 0;
    return 0;
}

static int recording_set(int blocked) {
    int err = sysfs_write(BLOCK_RECORDING_PATH, blocked ? "1" : "0");
    if (err) {
        sysfs_error(BLOCK_RECORDING_PATH, err);
        return -1;
    }
    return 0;
}

static void recording_print_status(int blocked) {
    printf("Recording: %s (webcam %s)\n",
           blocked ? "Blocked" : "Unblocked",
           blocked ? "disabled" : "enabled");
}

static void recording_help(void) {
    printf("Usage: galaxyctl recording [action]\n\n");
    printf("Manages the 'Recording Block' firmware attribute.\n\n");
    printf("Actions:\n");
    printf("  (none)     Show the current status of 'Recording Block'\n");
    printf("  block      Block recording, disabling the webcam\n");
    printf("  unblock    Unblock recording, enabling the webcam\n");
    printf("  toggle     Toggle 'Recording Block'\n");
    printf("\n");
    printf("block, unblock and toggle require sudo.\n");
}

// ====================================================
// Command
// ====================================================

int recording_command(int argc, char *argv[]) {
    int blocked;

    if (argc < 1) {
        if (recording_get(&blocked) != 0)
            return 1;
        recording_print_status(blocked);
        return 0;
    }

    const char *action = argv[0];

    if (strcmp(action, "help") == 0 || strcmp(action, "--help") == 0 || strcmp(action, "-h") == 0) {
        recording_help();
        return 0;
    }

    if (strcmp(action, "block") == 0) {
        blocked = 1;
    } else if (strcmp(action, "unblock") == 0) {
        blocked = 0;
    } else if (strcmp(action, "toggle") == 0) {
        if (recording_get(&blocked) != 0)
            return 1;
        blocked = !blocked;
    } else {
        fprintf(stderr, "Unknown recording action: %s\n\n", action);
        recording_help();
        return 1;
    }

    if (recording_set(blocked) != 0)
        return 1;

    recording_print_status(blocked);
    return 0;
}
