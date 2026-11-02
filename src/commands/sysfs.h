#ifndef SYSFS_H
#define SYSFS_H

// All functions return 0 on success or -errno on failure.

// Read the first line of a sysfs file into buffer, without trailing newline
int sysfs_read(const char *path, char *buffer, unsigned long size);

// Read a sysfs file containing a single integer
int sysfs_read_long(const char *path, long *value);

int sysfs_write(const char *path, const char *value);

// Remember the command line, used to suggest 'sudo <command>' on permission errors
void sysfs_set_command(int argc, char *argv[]);

// Print a user friendly message for an error returned by the functions above
void sysfs_error(const char *path, int err);

#endif
