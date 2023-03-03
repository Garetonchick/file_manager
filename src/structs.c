#include "structs.h"

#include "constants.h"
#include "exit.h"
#include "logger.h"

#include <curses.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>

void FetchWindowInfo(FileManagerState* st) {
    getmaxyx(stdscr, st->win_height, st->win_width);   
}

void InitFileManagerState(FileManagerState* st) {
    getcwd(st->current_path, PATH_MAX);
    st->current_path_len = strlen(st->current_path);
    st->copy_path[0] = '\0';
    st->cut_file = false;
    st->show_hidden = false;

    st->items = GetDirItemsList(st->current_path, false);
    qsort(st->items.items, st->items.size, sizeof(DirItem), DirItemCmp);

    st->selected_idx = 0;
    st->first_item_idx = 0;

    FetchWindowInfo(st);

    Log("Initialized file manager state\n");
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

bool UpdateDirItemsList(DirItemsList *items_list, const char* new_path, bool include_hidden) {
    DestroyDirItemList(*items_list);
    errno = 0;
    *items_list = GetDirItemsList(new_path, include_hidden);

    if(!items_list->items) {
        return false;
    }

    qsort(items_list->items, items_list->size, sizeof(DirItem), DirItemCmp);

    return true;
}

DirItemsList GetDirItemsList(const char *path, bool include_hidden) {
    static const DirItemsList kBadList = {.items = NULL, .size = 0};

    DIR *dir = opendir(path);

    if (!dir) {
        return kBadList;
    }

    int path_len = strlen(path);
    DirItemsList list = {.items = NULL, .size = 0};
    int capacity = 0;
    static struct stat stat_buf;
    static char path_buf[PATH_MAX + 1] = {0};
    struct dirent *dir_member = NULL;

    sprintf(path_buf, "%s", path);

    errno = 0;
    while ((dir_member = readdir(dir))) {
        if (strcmp(dir_member->d_name, ".") == 0) {
            continue;
        }

        if(!include_hidden && strcmp(dir_member->d_name, "..") != 0 && *dir_member->d_name == '.') {
            continue;
        }

        const char *format = path[path_len - 1] == '/' ? "%s" : "/%s";

        sprintf(path_buf + path_len, format, dir_member->d_name);
        errno = 0;

        if (lstat(path_buf, &stat_buf) < 0) {
            Log("Couldn't get file stat: %s\n", path_buf);
            Log("Reason: %s\n", strerror(errno));
            errno = 0;
            continue;
        }

        int name_len = strlen(dir_member->d_name);
        char *name = (char *)malloc(name_len + 1);

        if(!name) {
            FailExit();
        }

        strcpy(name, dir_member->d_name);

        int new_item_idx = list.size;
        ++list.size;

        if (list.size > capacity) {
            capacity = capacity ? capacity * 2 : 2;
            list.items =
                (DirItem *)realloc(list.items, capacity * sizeof(DirItem));

            if(!list.items) {
                FailExit();
            }
        }

        list.items[new_item_idx].name = name;
        list.items[new_item_idx].size = stat_buf.st_size; 

        int ftype = FILE_TYPE_OTHER;

        if(S_ISREG(stat_buf.st_mode)) {
            ftype =  FILE_TYPE_REGULAR; 
        } else if(S_ISDIR(stat_buf.st_mode)) {
            ftype = FILE_TYPE_DIR; 
        } else if(S_ISLNK(stat_buf.st_mode)) {
            ftype = FILE_TYPE_SYMBOLIC_LINK; 
        } else if(S_ISFIFO(stat_buf.st_mode)) {
            ftype = FILE_TYPE_FIFO;
        } 

        list.items[new_item_idx].type = ftype;
    }

    if(errno) {
        closedir(dir);
        DestroyDirItemList(list);
        return kBadList;
    }

    closedir(dir);

    return list;
}

void ReloadCurrentDir(FileManagerState* st) {
    UpdateDirItemsList(&st->items, st->current_path, st->show_hidden);

    if(st->selected_idx >= st->items.size) {
        st->selected_idx = st->items.size - 1;
    }

    if(st->first_item_idx > st->selected_idx) {
        st->first_item_idx = st->selected_idx;
    }
}