#include "actions.h"

#include "constants.h"
#include "structs.h"
#include "utils.h"
#include "global_buf.h"

#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// Helpers

void ReloadCurrentDir(FileManagerState* st) {
    UpdateDirItemsList(&st->items, st->current_path);

    if(st->selected_idx >= st->items.size) {
        --st->selected_idx;
    }
}

// Actions

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
        ReloadCurrentDir(st);
    }
} 

void CutCopyFile(FileManagerState* st) {
    CopyFile(st);
    st->cut_file = true;
}

void CopyFile(FileManagerState* st) {
    if(!st->items.items[st->selected_idx].is_dir) {
        ConcatPaths(st->current_path, st->items.items[st->selected_idx].name, st->copy_path);
        st->cut_file = false;
    }
}

void JustPasteFile(FileManagerState* st) {
    ConcatPaths(st->current_path, GetFileName(st->copy_path), g_buf);

    if(strcmp(st->copy_path, g_buf) == 0) {
        return;
    }

    int in_fd = open(st->copy_path, O_RDONLY);
    int out_fd = open(g_buf, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    if(in_fd < 0) {
        perror(st->copy_path);
        exit(1);
    }

    if(out_fd < 0) {
        perror(g_buf);
        exit(1);
    }

    int bytes_read = 0;

    while((bytes_read = read(in_fd, g_read_buf, READ_BUF_SIZE)) > 0) {
        if(write(out_fd, g_read_buf, bytes_read) < bytes_read) {
            perror("write");
            exit(1);
        }
    }

    if(bytes_read < 0) {
        perror("read");
        exit(1);
    }

    close(in_fd);
    close(out_fd);

    struct stat in_stat;

    if (lstat(st->copy_path, &in_stat) == -1) {
        exit(1);
    }

    chmod(g_buf, in_stat.st_mode);

    st->copy_path[0] = '\0';
}

void PasteAndDeleteFile(FileManagerState* st) {
    ConcatPaths(st->current_path, GetFileName(st->copy_path), g_buf);
    rename(st->copy_path, g_buf);
    st->copy_path[0] = '\0';
}

void PasteFile(FileManagerState* st) {
    if(*st->copy_path) {
        if(st->cut_file) {
            PasteAndDeleteFile(st);
        } else {
            JustPasteFile(st);
        }

        ReloadCurrentDir(st);
    }
}