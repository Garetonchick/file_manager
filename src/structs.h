#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

typedef struct DirItem {
    char *name;
    size_t size;
    int type;
} DirItem;

typedef struct DirItemsList {
    DirItem *items;
    int size;
} DirItemsList;

typedef struct FileManagerState {
    int win_height;
    int win_width;
    DirItemsList items;
    char current_path[PATH_MAX + 1];
    char copy_path[PATH_MAX + 1];
    bool cut_file;
    bool show_hidden;
    int current_path_len;
    int selected_idx;
    int first_item_idx;
} FileManagerState;

void InitFileManagerState(FileManagerState* st);
void FetchWindowInfo(FileManagerState* st);

int DirItemCmp(const void *item1, const void *item2);
DirItemsList GetDirItemsList(const char *path, bool include_hidden);
void DestroyDirItemList(DirItemsList list);
void UpdateDirItemsList(DirItemsList *items_list, const char* new_path, bool include_hidden);
void ReloadCurrentDir(FileManagerState* st);