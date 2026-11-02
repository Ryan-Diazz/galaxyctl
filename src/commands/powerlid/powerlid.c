#include <stdio.h>
#include <string.h>

#include "powerlid.h"
#include "sysfs.h"

#define POWER_LID_PATH "/sys/class/firmware-attributes/samsung-galaxybook/attributes/power_on_lid_open/current_value"

// ====================================================
// Helpers
// ====================================================
static int get(int *enabled) {
    long value;
    int err = sysfs_read_long(POWER_LID_PATH, &value);
    if (err) {
        sysfs_error(POWER_LID_PATH, err);
        return -1;
    }

    *enabled = value != 0;
    return 0;
}

static int set(int enabled) {
    int err = sysfs_write(POWER_LID_PATH, enabled ? "1" : "0");
    if (err) {
        sysfs_error(POWER_LID_PATH, err);
        return -1;
    }
    return 0;
}

static void print_status(int enabled) {
    printf("Power On Lid Open: %s \n",
           enabled ? "enabled" : "disabled");
}

static void help(void) {
    printf("Usage: galaxyctl powerlid [action]\n\n");
    printf("Manages the 'Power On Lid Open' firmware attribute.\n\n");
    printf("Actions:\n");
    printf("  (none)     Show the current status of 'Power On Lid Open'\n");
    printf("  enable     Power on the laptop when the lid is opened\n");
    printf("  disable    Do now power on the laptop when the lid is opened");
    printf("  toggle     Toggle 'Power On Lid Open'\n");
    printf("\n");
    printf("enable, disable and toggle require sudo.\n");
}

// ====================================================
// Command
// ====================================================

int powerlid_command(int argc, char *argv[]) {
    int enabled;

    if (argc < 1) {
        if (get(&enabled) != 0)
            return 1;
        print_status(enabled);
        return 0;
    }

    const char *action = argv[0];

    if (strcmp(action, "help") == 0 || strcmp(action, "--help") == 0 || strcmp(action, "-h") == 0) {
        help();
        return 0;
    }

    if (strcmp(action, "enable") == 0) {
        enabled = 1;
    } else if (strcmp(action, "disable") == 0) {
        enabled = 0;
    } else if (strcmp(action, "toggle") == 0) {
        if (get(&enabled) != 0)
            return 1;
        enabled = !enabled;
    } else {
        fprintf(stderr, "Unknown powerlid action: %s\n\n", action);
        help();
        return 1;
    }

    if (set(enabled) != 0)
        return 1;

    print_status(enabled);
    return 0;
}
