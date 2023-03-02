#include "utils.h"

#include <string.h>
#include <limits.h>
#include <stdio.h>

void ConcatPaths(const char* path, const char* local_path, char* out) {
    const char *format =
        path[strlen(path) - 1] == '/' ? "%s%s" : "%s/%s";
    snprintf(out, PATH_MAX, format, path, local_path);
}

const char* GetFileName(const char* path) {
    const char* last_slash = path;

    while(*path) {
        if(*path == '/') {
            last_slash = path;
        }

        ++path;
    }

    return last_slash + 1;
}

const char* GetFileExtension(const char* path) {
    int path_len = strlen(path);
    path += path_len; 
    const char* last_dot = path;

    while(path_len--) {
        --path;

        if(*path == '.') {
            last_dot = path;
            break;
        }
    }

    return last_dot;
}

void mvprintfw(int row, int col, const char* format, ...) { // NOLINT
    va_list args;
    va_start(args, format);
    vsnprintf(g_buf, GLOBAL_BUF_SIZE, format, args);
    va_end(args);
    mvprintw(row, col, g_buf);
}