#include <assert.h>
#include <curses.h>
#include <dirent.h>
#include <linux/limits.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "debug.h"

enum {
    ROWS_NUM = 24,
    COLS_NUM = 80,
    SUBWINDOW_ROW_OFFSET = 1,
    SUBWINDOW_COL_OFFSET = 2,
    SUBWINDOW_ROWS = 23,
    SUBWINDOW_COLS = 78,
    DIR_COLOR_PAIR = 1
};

typedef struct DirItem {
    char *name;
    bool is_dir;
} DirItem;

int DirItemCmp(const void *item1, const void *item2) {
    return strcmp(((DirItem *)item1)->name, ((DirItem *)item2)->name);
}

typedef struct DirItemsList {
    DirItem *items;
    int size;
} DirItemsList;

void DestroyDirItemList(DirItemsList list) {
    for(int i = 0; i < list.size; ++i) {
        free(list.items[i].name);
    }

    free(list.items);
}

DirItemsList GetDirItemsList(const char *path) {
    DIR *dir = opendir(path);
    PRINT_ERR("Opened dir\n");

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

    PRINT_ERR("Wrote path in path buf: %s\n", path_buf);

    while ((dir_member = readdir(dir))) {
        PRINT_ERR("Reading dir member: %s\n", dir_member->d_name);

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
        // fprintf(stderr, "Kek: %s\n", path_buf);

        if (list.size > capacity) {
            capacity = capacity ? capacity * 2 : 2;
            list.items =
                (DirItem *)realloc(list.items, capacity * sizeof(DirItem));
        }

        list.items[new_item_idx].name = name;
        list.items[new_item_idx].is_dir = S_ISDIR(stat_buf.st_mode);
    }

    goto get_dir_items_end;

get_dir_items_error:
    closedir(dir);
    exit(1);

get_dir_items_end:
    closedir(dir);

    return list;
}

void DisplayDirectoryContents(DirItemsList content, int rows, int first_item_idx) {
    int lines_to_print = content.size;

    if (lines_to_print > rows) {
        lines_to_print = rows;
    }

    for (int i = 0; i < lines_to_print; ++i) {
        DirItem* item = &content.items[first_item_idx + i];

        if(item->is_dir) {
            attron(COLOR_PAIR(DIR_COLOR_PAIR));
            mvprintw(SUBWINDOW_ROW_OFFSET + i, SUBWINDOW_COL_OFFSET, item->name);
            attroff(COLOR_PAIR(DIR_COLOR_PAIR));
        } else {
            mvprintw(SUBWINDOW_ROW_OFFSET + i, SUBWINDOW_COL_OFFSET, item->name);
        }
    }
}

void DisplayArrow(int row) {
    mvprintw(row, 0, "->");
}

void DisplayHeader() {
    attron(A_REVERSE);

    move(0, 0);

    for (int i = 0; i < COLS_NUM; ++i) {
        printw(" ");
    }

    mvprintw(0, SUBWINDOW_COL_OFFSET, "Name");
    mvprintw(0, 50, "Size");
    mvprintw(0, 66, "Modified");
    attroff(A_REVERSE);
}

int main(void) {
    initscr();
    curs_set(0);
    keypad(stdscr, true);

    use_default_colors();
    start_color();

    init_pair(DIR_COLOR_PAIR, COLOR_CYAN, -1);

    refresh();

    char current_path[PATH_MAX + 1] = { 0 };
    strcpy(current_path, "/home/gareton");
    int path_len = strlen(current_path);

    DirItemsList items = GetDirItemsList(current_path);
    qsort(items.items, items.size, sizeof(DirItem), DirItemCmp);
    int selected_idx = 0;
    int first_item_idx = 0;

    int key = 0;

    do {
        switch (key) {
        case KEY_DOWN:
            if (selected_idx + 1 != items.size) {
                ++selected_idx;
            }

            if (selected_idx - first_item_idx >= SUBWINDOW_ROWS) {
                ++first_item_idx;
            }

            break;
        case KEY_UP:
            if (selected_idx != 0) {
                --selected_idx;
            }

            if (selected_idx < first_item_idx) {
                --first_item_idx;
            }

            break;
        case '\n': {
            if(items.items[selected_idx].is_dir) {
                if (strcmp(items.items[selected_idx].name, "..") == 0) {
                    while(current_path[--path_len] != '/') {
                    }

                    if(path_len != 0) {
                        current_path[path_len] = '\0';
                    } else {
                        path_len = 1;
                        current_path[path_len] = '\0';
                    }
                } else {
                    const char* format = current_path[path_len - 1] == '/' ? "%s" : "/%s";
                    snprintf(current_path + path_len, PATH_MAX, format, items.items[selected_idx].name); 
                }

                DestroyDirItemList(items);
                PRINT_ERR("Destroyed items\n");
                PRINT_ERR("Current path: %s\n", current_path);
                items = GetDirItemsList(current_path);
                PRINT_ERR("Got items\n");
                qsort(items.items, items.size, sizeof(DirItem), DirItemCmp);
                path_len = strlen(current_path);
                selected_idx = 0;
                first_item_idx = 0;
            }
            break;
            }
        default:
            break;
        }


        int arrow_row = SUBWINDOW_ROW_OFFSET + selected_idx - first_item_idx;

        clear();
        mvprintw(26, 1, current_path);
        DisplayDirectoryContents(items, SUBWINDOW_ROWS, first_item_idx);
        DisplayArrow(arrow_row);
        DisplayHeader();
        refresh();
    } while ((key = getch()) != 'q');

    endwin();
    return 0;
}
