#include "actions.h"

#include "constants.h"
#include "utils.h"
#include "global_buf.h"

#include <string.h>
#include <stdio.h>

void SelectFileBelowAction(FileManagerState* st) {
    if (st->selected_idx + 1 != st->items.size) {
        ++st->selected_idx;
    }

    if (st->selected_idx - st->first_item_idx >= SUBWINDOW_ROWS) {
        ++st->first_item_idx;
    }
}

void SelectFileAboveAction(FileManagerState* st) {
    if (st->selected_idx != 0) {
        --st->selected_idx;
    }

    if (st->selected_idx < st->first_item_idx) {
        --st->first_item_idx;
    }
}

void EnterDirAction(FileManagerState* st) {
    if (st->items.items[st->selected_idx].is_dir) {
        if (strcmp(st->items.items[st->selected_idx].name, "..") == 0) {
            while (st->current_path[--st->current_path_len] != '/') {
            }

            if (st->current_path_len != 0) {
                st->current_path[st->current_path_len] = '\0';
            } else {
                st->current_path_len = 1;
                st->current_path[st->current_path_len] = '\0';
            }
        } else {
            const char *format =
                st->current_path[st->current_path_len - 1] == '/' ? "%s" : "/%s";
            snprintf(st->current_path + st->current_path_len, PATH_MAX, format,
                        st->items.items[st->selected_idx].name);
        }

        UpdateDirItemsList(&st->items, st->current_path);
        st->current_path_len = strlen(st->current_path);
        st->selected_idx = 0;
        st->first_item_idx = 0;
    }
}

void DeleteDirMemberAction(FileManagerState* st) {
    if(st->selected_idx) {
        ConcatPaths(st->current_path, st->items.items[st->selected_idx].name, g_buf);
        remove(g_buf);
        UpdateDirItemsList(&st->items, st->current_path);

        if(st->selected_idx >= st->items.size) {
            --st->selected_idx;
        }
    }
} 