#include "utils.h"

#include <string.h>
#include <limits.h>
#include <stdio.h>

void ConcatPaths(const char* path, const char* local_path, char* out) {
    const char *format =
        path[strlen(path) - 1] == '/' ? "%s%s" : "%s/%s";
    snprintf(out, PATH_MAX, format, path, local_path);
}