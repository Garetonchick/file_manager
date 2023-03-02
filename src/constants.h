#pragma once

// Buffer sizes
enum {
    GLOBAL_BUF_SIZE = 4444,
    READ_BUF_SIZE = 4096,
};

// Windows info
enum {
    ROWS_NUM = 24,
    COLS_NUM = 80,
    SUBWINDOW_ROW_OFFSET = 1,
    SUBWINDOW_COL_OFFSET = 2,
    SUBWINDOW_ROWS = 23,
    SUBWINDOW_COLS = 78,
    SIZE_INFO_COLUMN = 50,
    MODIFIED_INFO_COLUMN = 66,
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