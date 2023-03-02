#pragma once
#include "constants.h"

typedef struct LibEntry {
    char* extension;
    char* libname;
} LibEntry;

void InitLibList(const char* libs_dir);
void DestroyLibList();
void OpenFile(const char* path, const char* current_path);
