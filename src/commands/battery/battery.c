#define _POSIX_C_SOURCE 200809L

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "battery.h"
#include "sysfs.h"

#define POWER_SUPPLY_DIR "/sys/class/power_supply"
#define LIMIT_MIN 60
#define LIMIT_MAX 100

// Path of the battery directory, e.g. /sys/class/power_supply/BAT1
static char battery_dir[sizeof POWER_SUPPLY_DIR + 256];

// ====================================================
// Helpers
// ====================================================

// Find the first power supply whose type is "Battery"
static int find_battery(void) {
    DIR *dir = opendir(POWER_SUPPLY_DIR);
    if (!dir) {
        sysfs_error(POWER_SUPPLY_DIR, -errno);
        return -1;
    }

    struct dirent *entry;
    int found = 0;
    while (!found && (entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.')
            continue;

        char path[512];
        char type[32];
        snprintf(path, sizeof path, "%s/%s/type", POWER_SUPPLY_DIR, entry->d_name);
        if (sysfs_read(path, type, sizeof type) == 0 && strcmp(type, "Battery") == 0) {
            snprintf(battery_dir, sizeof battery_dir, "%s/%s", POWER_SUPPLY_DIR, entry->d_name);
            found = 1;
        }
    }
    closedir(dir);

    if (!found) {
        fprintf(stderr, "No battery found in %s\n", POWER_SUPPLY_DIR);
        return -1;
    }
    return 0;
}

static void attr_path(char *path, size_t size, const char *attr) {
    snprintf(path, size, "%s/%s", battery_dir, attr);
}

// Optional attributes: return 0 on success, non zero when missing
static int read_str(const char *attr, char *buffer, unsigned long size) {
    char path[512];
    attr_path(path, sizeof path, attr);
    return sysfs_read(path, buffer, size);
}

static int read_long(const char *attr, long *value) {
    char path[512];
    attr_path(path, sizeof path, attr);
    return sysfs_read_long(path, value);
}

static int get_limit(long *limit) {
    char path[512];
    attr_path(path, sizeof path, "charge_control_end_threshold");

    int err = sysfs_read_long(path, limit);
    if (err) {
        sysfs_error(path, err);
        return -1;
    }
    return 0;
}

static int set_limit(long limit) {
    char path[512];
    char value[16];
    attr_path(path, sizeof path, "charge_control_end_threshold");
    snprintf(value, sizeof value, "%ld", limit);

    int err = sysfs_write(path, value);
    if (err) {
        sysfs_error(path, err);
        return -1;
    }
    return 0;
}

// Parse a whole number in [LIMIT_MIN, LIMIT_MAX], a trailing '%' is allowed
static int parse_limit(const char *text, long *limit) {
    char *end;
    errno = 0;
    long value = strtol(text, &end, 10);

    if (end == text || errno)
        return -1;
    if (*end == '%')
        end++;
    if (*end != '\0')
        return -1;
    if (value < LIMIT_MIN || value > LIMIT_MAX)
        return -1;

    *limit = value;
    return 0;
}

static void print_limit(long limit) {
    printf("Charge limit: %ld%%\n", limit);
}

// ====================================================
// Output
// ====================================================

static void print_status(void) {
    char status[32];
    long capacity, limit;

    if (read_long("capacity", &capacity) == 0)
        printf("Charge:       %ld%%\n", capacity);
    if (read_str("status", status, sizeof status) == 0)
        printf("Status:       %s\n", status);
    if (read_long("charge_control_end_threshold", &limit) == 0)
        print_limit(limit);
}

static void print_health(void) {
    char text[64];
    long cycles, full, design;

    // Capacity is reported either as charge (µAh) or energy (µWh)
    const char *unit = "mAh";
    int have_full = read_long("charge_full", &full) == 0;
    int have_design = read_long("charge_full_design", &design) == 0;
    if (!have_full || !have_design) {
        unit = "mWh";
        have_full = read_long("energy_full", &full) == 0;
        have_design = read_long("energy_full_design", &design) == 0;
    }

    printf("Model:            %s\n", read_str("model_name", text, sizeof text) == 0 ? text : "unknown");
    printf("Manufacturer:     %s\n", read_str("manufacturer", text, sizeof text) == 0 ? text : "unknown");
    printf("Technology:       %s\n", read_str("technology", text, sizeof text) == 0 ? text : "unknown");

    if (read_long("cycle_count", &cycles) == 0)
        printf("Cycle count:      %ld\n", cycles);
    else
        printf("Cycle count:      unknown\n");

    if (have_full && have_design && design > 0) {
        printf("Health (SOH):     %.1f%%\n", 100.0 * (double)full / (double)design);
        printf("Full capacity:    %ld %s\n", full / 1000, unit);
        printf("Design capacity:  %ld %s\n", design / 1000, unit);
    } else {
        printf("Health (SOH):     unknown\n");
    }
}

static void help(void) {
    printf("Usage: galaxyctl battery [action]\n\n");
    printf("Shows battery information and manages the charge limit.\n\n");
    printf("Actions:\n");
    printf("    (none)           Show charge, status and charge limit\n");
    printf("    health           Show model, technology, cycle count and health\n");
    printf("    limit            Show the current charge limit\n");
    printf("    limit <%d-%d>   Stop charging at this percentage\n", LIMIT_MIN, LIMIT_MAX);
    printf("\n");
    printf("limit <%d-%d> requires sudo.\n", LIMIT_MIN, LIMIT_MAX);
}

// ====================================================
// Command
// ====================================================

int battery_command(int argc, char *argv[]) {
    if (argc >= 1) {
        const char *action = argv[0];
        if (strcmp(action, "help") == 0 || strcmp(action, "--help") == 0 || strcmp(action, "-h") == 0) {
            help();
            return 0;
        }
    }

    if (find_battery() != 0)
        return 1;

    if (argc < 1) {
        print_status();
        return 0;
    }

    const char *action = argv[0];

    if (strcmp(action, "health") == 0) {
        print_health();
        return 0;
    }

    if (strcmp(action, "limit") == 0) {
        long limit;

        if (argc < 2) {
            if (get_limit(&limit) != 0)
                return 1;
            print_limit(limit);
            return 0;
        }

        if (parse_limit(argv[1], &limit) != 0) {
            fprintf(stderr, "Invalid charge limit: %s (must be a number between %d and %d)\n",
                    argv[1], LIMIT_MIN, LIMIT_MAX);
            return 1;
        }

        if (set_limit(limit) != 0)
            return 1;

        print_limit(limit);
        return 0;
    }

    fprintf(stderr, "Unknown battery action: %s\n\n", action);
    help();
    return 1;
}
