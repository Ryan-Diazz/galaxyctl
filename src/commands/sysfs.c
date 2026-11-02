#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sysfs.h"

int sysfs_read(const char *path, char *buffer, unsigned long size) {
    if (size == 0)
        return -EINVAL;

    int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0)
        return -errno;

    // sysfs attributes are at most one page and are returned in a single read
    ssize_t n = read(fd, buffer, size - 1);
    int err = errno;
    close(fd);

    if (n < 0)
        return -err;

    buffer[n] = '\0';
    buffer[strcspn(buffer, "\n")] = '\0';
    return 0;
}

int sysfs_read_long(const char *path, long *value) {
    char buffer[64];
    int err = sysfs_read(path, buffer, sizeof(buffer));
    if (err)
        return err;

    char *end;
    errno = 0;
    long v = strtol(buffer, &end, 10);
    if (end == buffer || errno)
        return -EINVAL;

    *value = v;
    return 0;
}

int sysfs_write(const char *path, const char *value) {
    int fd = open(path, O_WRONLY | O_CLOEXEC);
    if (fd < 0)
        return -errno;

    // The driver validates the value in one write call, errors come back here
    size_t len = strlen(value);
    ssize_t n = write(fd, value, len);
    int err = errno;
    close(fd);

    if (n < 0)
        return -err;
    if ((size_t)n != len)
        return -EIO;
    return 0;
}

static int command_argc;
static char **command_argv;

void sysfs_set_command(int argc, char *argv[]) {
    command_argc = argc;
    command_argv = argv;
}

void sysfs_error(const char *path, int err) {
    switch (-err) {
    case EACCES:
    case EPERM:
        fprintf(stderr, "Permission denied: This command requires root!\n");
        fprintf(stderr, "Try: sudo");
        for (int i = 0; i < command_argc; i++)
            fprintf(stderr, " %s", command_argv[i]);
        fprintf(stderr, "\n");
        break;
    case ENOENT:
        fprintf(stderr, "Not supported: %s does not exist.\n", path);
        fprintf(stderr, "Is the samsung-galaxybook driver loaded?\n");
        break;
    case ENXIO:
        fprintf(stderr, "Firmware rejected the request: %s\n", path);
        fprintf(stderr, "Try reloading the driver: "
                        "sudo modprobe -r samsung-galaxybook && sudo modprobe samsung-galaxybook\n");
        break;
    case EINVAL:
        fprintf(stderr, "Invalid value for %s\n", path);
        break;
    default:
        fprintf(stderr, "Error: %s: %s\n", path, strerror(-err));
    }
}
