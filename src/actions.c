#include "actions.h"

#include "constants.h"
#include "display.h"
#include "exit.h"
#include "extensions.h"
#include "logger.h"
#include "structs.h"
#include "utils.h"
#include "global_buf.h"

#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

void NormalizeArrow(FileManagerState* st) {
    if (st->selected_idx >= st->items.size) {
        st->selected_idx = st->items.size - 1;
    }

    if(st->first_item_idx > st->selected_idx) {
        st->first_item_idx = st->selected_idx;
    }

    while(!CanArrowBeDisplayed(st)) {
        ++st->first_item_idx;
    }
}

void SelectFileBelowAction(FileManagerState* st) {
    if (st->selected_idx + 1 != st->items.size) {
        ++st->selected_idx;
    }

    NormalizeArrow(st);
}

void SelectFileAboveAction(FileManagerState* st) {
    if (st->selected_idx != 0) {
        --st->selected_idx;
    }

    NormalizeArrow(st);
}

void EnterDirAction(FileManagerState* st) {
    char* old_path = AllocCopy(st->current_path);

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
        sprintf(st->current_path + st->current_path_len, format, st->items.items[st->selected_idx].name);
    }

    errno = 0;

    if(!UpdateDirItemsList(&st->items, st->current_path, st->show_hidden)) {
        Log("Couldn't enter directory: %s\n", st->current_path);
        Log("Reason: %s\n", strerror(errno));
        strcpy(st->current_path, old_path);

        st->items.items = NULL;
        st->items.size = 0;

        if(!UpdateDirItemsList(&st->items, st->current_path, st->show_hidden)) {
            Log("Couldn't renter directory: %s\n", st->current_path);
            Log("Reason: %s\n", strerror(errno));
            FailExit();
        }

        return;
    }

    st->current_path_len = strlen(st->current_path);
    st->selected_idx = 0;
    st->first_item_idx = 0;
}

void EnterPressedAction(FileManagerState* st) {
    if (st->items.items[st->selected_idx].type == FILE_TYPE_DIR) {
        EnterDirAction(st);
    } else {
        char* path_to_file = AllocConcatPaths(st->current_path, st->items.items[st->selected_idx].name);
        OpenFile(path_to_file);
    }
}

void DeleteDirMemberAction(FileManagerState* st) {
    if(st->selected_idx) {
        ConcatPaths(st->current_path, st->items.items[st->selected_idx].name, g_buf);
        errno = 0;

        if(remove(g_buf) == -1) {
            Log("Couldn't remove file: %s\nReason: %s\n", g_buf, strerror(errno));
        } else {
            ReloadCurrentDir(st);
        }
    }
} 

void CutCopyFile(FileManagerState* st) {
    CopyFile(st);
    st->cut_file = true;
}

void CopyFile(FileManagerState* st) {
    int ftype = st->items.items[st->selected_idx].type;
    if(ftype == FILE_TYPE_REGULAR || ftype == FILE_TYPE_SYMBOLIC_LINK || ftype == FILE_TYPE_FIFO) {
        ConcatPaths(st->current_path, st->items.items[st->selected_idx].name, st->copy_path);
        st->cut_file = false;
    }
}

void JustPasteFile(FileManagerState* st) {
    ConcatPaths(st->current_path, GetFileName(st->copy_path), g_buf);

    if(strcmp(st->copy_path, g_buf) == 0) {
        goto just_paste_file_finish;
    }

    errno = 0;
    int in_fd = open(st->copy_path, O_RDONLY);

    if(in_fd < 0) {
        Log("Couldn't open file for copy: %s\n", st->copy_path);
        Log("Reason: %s\n", strerror(errno));
        goto just_paste_file_finish;
    }

    errno = 0;
    int out_fd = open(g_buf, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    if(out_fd < 0) {
        Log("Couldn't open file for paste: %s\n", g_buf);
        Log("Reason: %s\n", strerror(errno));
        goto just_paste_file_finish;
    }

    int bytes_read = 0;
    errno = 0;

    while((bytes_read = read(in_fd, g_read_buf, READ_BUF_SIZE)) > 0) {
        if(write(out_fd, g_read_buf, bytes_read) < bytes_read) {
            Log("Copy-Paste was interrupted\n");
            Log("Reason: %s\n", strerror(errno));
            goto just_paste_file_finish;
        }
    }

    if(bytes_read < 0) {
        Log("Copy-Paste was interrupted\n");
        Log("Reason: %s\n", strerror(errno));
        goto just_paste_file_finish;
    }

    close(in_fd);
    close(out_fd);

    struct stat in_stat;
    errno = 0;

    if (lstat(st->copy_path, &in_stat) == -1) {
        Log("Couldn't access original file stats\n");
        Log("Reason: %s\n", strerror(errno));
        goto just_paste_file_finish;
    }

    errno = 0;

    if(chmod(g_buf, in_stat.st_mode) < 0) {
        Log("Couldn't access original file stats\n");
        Log("Reason: %s\n", strerror(errno));
        goto just_paste_file_finish;
    }

just_paste_file_finish:
    st->copy_path[0] = '\0';
}

void PasteAndDeleteFile(FileManagerState* st) {
    ConcatPaths(st->current_path, GetFileName(st->copy_path), g_buf);
    errno = 0;

    if(rename(st->copy_path, g_buf) < 0) {
        Log("Failed to move file: %s\n", st->copy_path);
        Log("Reason: %s\n", strerror(errno));
    }

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

void SwitchShowingHidden(FileManagerState* st) {
    st->show_hidden = !st->show_hidden;
    ReloadCurrentDir(st);
}