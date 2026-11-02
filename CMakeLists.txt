cmake_minimum_required(VERSION 4.3)
project(galaxyctl C)

set(CMAKE_C_STANDARD 11)

include_directories(src)
include_directories(src/commands)

add_executable(galaxyctl
        src/commands/commands.c
        src/commands/commands.h
        src/commands/powerlid/powerlid.c
        src/commands/powerlid/powerlid.h
        src/commands/recording/recording.c
        src/commands/recording/recording.h
        src/commands/sysfs.c
        src/commands/sysfs.h
        src/main.c)
