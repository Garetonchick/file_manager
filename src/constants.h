#pragma once
#define SIZE_INFO_RELATIVE_OFFSET 0.7f

// Buffer sizes
#include <linux/limits.h>
enum {
    GLOBAL_BUF_SIZE = 4444,
    READ_BUF_SIZE = 4096,
    LIBS_LIST_SIZE = 4444
};

// Colors
enum {
    DIR_COLOR_PAIR = 1,
    SYMLINK_COLOR_PAIR = 2,
    FIFO_COLOR_PAIR = 3
};

// File types
enum {
    FILE_TYPE_REGULAR,
    FILE_TYPE_DIR,
    FILE_TYPE_SYMBOLIC_LINK,
    FILE_TYPE_FIFO,
    FILE_TYPE_OTHER
};

extern char g_path_to_program[PATH_MAX + 1];
extern char g_argv0[PATH_MAX + 1];