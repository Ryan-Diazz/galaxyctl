#include <stdio.h>
#include <string.h>

#include "profile.h"
#include "sysfs.h"

#define PATH "/sys/firmware/acpi/platform_profile"
#define OPTIONS "/sys/firmware/acpi/platform_profile_choices"

// ====================================================
// Helpers
// ====================================================
static int get(char *profile, unsigned long size) {
    int err = sysfs_read(PATH, profile, size);
    if (err) {
        sysfs_error(PATH, err);
        return -1;
    }
    return 0;
}

static int set(const char *profile) {
    int err = sysfs_write(PATH, profile);
    if (err) {
        sysfs_error(PATH, err);
        return -1;
    }
    return 0;
}

// Read the space separated list of supported profiles
static int get_choices(char *choices, unsigned long size) {
    int err = sysfs_read(OPTIONS, choices, size);
    if (err) {
        sysfs_error(OPTIONS, err);
        return -1;
    }
    return 0;
}

// Check whether profile is one of the words in choices
static int is_choice(const char *choices, const char *profile) {
    size_t len = strlen(profile);
    const char *p = choices;

    if (len == 0)
        return 0;

    while (*p) {
        while (*p == ' ')
            p++;

        size_t word = strcspn(p, " ");
        if (word == len && strncmp(p, profile, len) == 0)
            return 1;

        p += word;
    }
    return 0;
}

static void print_choices(const char *choices) {
    const char *p = choices;

    while (*p) {
        while (*p == ' ')
            p++;

        size_t word = strcspn(p, " ");
        if (word > 0)
            printf("    %.*s\n", (int)word, p);

        p += word;
    }
}

static void print_status(const char *profile) {
    printf("current power profile: %s\n", profile);
}

static void help(const char *choices) {
    printf("Usage: galaxyctl profile [action]\n\n");
    printf("Manages the 'Power Profile' on a galaxy book.\n");
    printf("Actions:\n");
    printf("    (none)     Show the current power profile\n");
    printf("    <option>   Set power profile to the option\n");
    printf("<option> requires sudo.\n");

    printf("\n");
    printf("Options:\n");
    if (choices)
        print_choices(choices);
    else
        printf("    (unable to read: '%s')\n", OPTIONS);
    printf("\n");
}

// ====================================================
// Command
// ====================================================

int profile_command(int argc, char *argv[]) {
    char profile[64];
    char choices[256];

    if (argc < 1) {
        if (get(profile, sizeof profile) != 0)
            return 1;
        print_status(profile);
        return 0;
    }

    const char *action = argv[0];
    int have_choices = get_choices(choices, sizeof choices) == 0;

    if (strcmp(action, "help") == 0 || strcmp(action, "--help") == 0 || strcmp(action, "-h") == 0) {
        help(have_choices ? choices : NULL);
        return 0;
    }

    if (!have_choices)
        return 1;

    if (!is_choice(choices, action)) {
        fprintf(stderr, "Unknown power profile: %s\n\n", action);
        help(choices);
        return 1;
    }

    if (set(action) != 0)
        return 1;

    print_status(action);
    return 0;
}
