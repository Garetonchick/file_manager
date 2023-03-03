#include "strings_storage.h"

#include "exit.h"

#include <stdlib.h>

static struct StrStorage {
    char** strs;
    int size;
    int capacity;
} g_str_storage; 

char* AllocString(int size) {
    char* str = calloc(size, 1);

    if(!str) {
        FailExit();
    }

    if(g_str_storage.capacity == g_str_storage.size) {
        g_str_storage.capacity = (g_str_storage.capacity + 1) * 2;
        g_str_storage.strs = realloc(g_str_storage.strs, g_str_storage.capacity * sizeof(char*));

        if(!g_str_storage.strs) {
            FailExit();
        }
    }

    g_str_storage.strs[g_str_storage.size] = str;
    ++g_str_storage.size;

    return str;
}

void ClearStorageStrings() {
    for(int i = 0; i < g_str_storage.size; ++i) {
        free(g_str_storage.strs[i]);
    }

    g_str_storage.size = 0;
}

void ClearStorage() {
    ClearStorageStrings();
    free(g_str_storage.strs);
    g_str_storage.capacity = 0;
}