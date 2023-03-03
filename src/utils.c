#include "utils.h"

#include "constants.h"
#include "global_buf.h"
#include "strings_storage.h"

#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <ncurses.h>

char* AllocCopy(const char* str) {
    char* copy = AllocString(strlen(str) + 1);
    strcpy(copy, str);
    return copy;
}

char* AllocConcatPaths(const char* path, const char* local_path) {
    int path_len = strlen(path);
    char last_path_c = path[path_len - 1];
    int full_path_len = path_len + (last_path_c == '/' ? 1 : 1) + strlen(local_path);

    char* out = AllocString(full_path_len + 1);

    const char *format = (last_path_c == '/' ? "%s%s" : "%s/%s");
    sprintf(out, format, path, local_path);

    return out;
}

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