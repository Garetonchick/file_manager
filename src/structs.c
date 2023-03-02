#include "structs.h"

#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

void InitFileManagerState(FileManagerState* st) {
    getcwd(st->current_path, PATH_MAX);
    st->current_path_len = strlen(st->current_path);
    st->copy_path[0] = '\0';
    st->cut_file = false;

    st->items = GetDirItemsList(st->current_path);
    qsort(st->items.items, st->items.size, sizeof(DirItem), DirItemCmp);

    st->selected_idx = 0;
    st->first_item_idx = 0;
}

int DirItemCmp(const void *item1, const void *item2) {
    return strcmp(((DirItem *)item1)->name, ((DirItem *)item2)->name);
}

void DestroyDirItemList(DirItemsList list) {
    for (int i = 0; i < list.size; ++i) {
        free(list.items[i].name);
    }

    free(list.items);
}

void UpdateDirItemsList(DirItemsList *items_list, const char* new_path) {
    DestroyDirItemList(*items_list);
    *items_list = GetDirItemsList(new_path);
    qsort(items_list->items, items_list->size, sizeof(DirItem), DirItemCmp);
}

DirItemsList GetDirItemsList(const char *path) {
    DIR *dir = opendir(path);

    if (!dir) {
        DirItemsList list = {.items = NULL, .size = 0};
        return list;
    }

    int path_len = strlen(path);
    DirItemsList list = {.items = NULL, .size = 0};
    int capacity = 0;
    static struct stat stat_buf;
    static char path_buf[PATH_MAX + 1] = {0};
    struct dirent *dir_member = NULL;

    if (snprintf(path_buf, PATH_MAX, "%s", path) >= PATH_MAX) {
        fprintf(stderr, "Exceeded max path length\n");
        goto get_dir_items_error;
    }

    while ((dir_member = readdir(dir))) {
        if (strcmp(dir_member->d_name, ".") == 0) {
            continue;
        }

        const char *format = path[path_len - 1] == '/' ? "%s" : "/%s";

        if (snprintf(path_buf + path_len, PATH_MAX - path_len, format,
                     dir_member->d_name) >= PATH_MAX - path_len) {
            fprintf(stderr, "Exceeded max path length\n");
            goto get_dir_items_error;
        }

        if (lstat(path_buf, &stat_buf) < 0) {
            fprintf(stderr, "Lstat for path \"%s\" failed\n", path_buf);
            goto get_dir_items_error;
        }

        int name_len = strlen(dir_member->d_name);
        char *name = (char *)malloc(name_len + 1);
        strcpy(name, dir_member->d_name);

        int new_item_idx = list.size;
        ++list.size;

        if (list.size > capacity) {
            capacity = capacity ? capacity * 2 : 2;
            list.items =
                (DirItem *)realloc(list.items, capacity * sizeof(DirItem));
        }

        list.items[new_item_idx].name = name;
        list.items[new_item_idx].is_dir = S_ISDIR(stat_buf.st_mode);
        list.items[new_item_idx].size = stat_buf.st_size; 
    }

    goto get_dir_items_end;

get_dir_items_error:
    closedir(dir);
    exit(1);

get_dir_items_end:
    closedir(dir);

    return list;
}