#include "debug.h"
#include "actions.h"
#include "display.h"
#include "constants.h"
#include "extensions.h"
#include "structs.h"
#include "utils.h"
#include "init.h"

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
#include <ctype.h>


void UpdateState(FileManagerState* st, int key) {
    switch (key) {
        case KEY_DOWN:
            SelectFileBelowAction(st); 
            break;

        case KEY_UP:
            SelectFileAboveAction(st);
            break;

        case '\n': 
            EnterPressedAction(st);
            break;
        
        case 'D':
            DeleteDirMemberAction(st);
            break;

        case 'C':
            CopyFile(st);
            break;

        case 'X':
            CutCopyFile(st);
            break;

        case 'V':
            PasteFile(st);
            break;
        
        case 'H':
            SwitchShowingHidden(st);
            break;

        default:
            break;
    }
}

int main(int args, char* argv[]) {
    if(!args) {
        return 1;
    }

    InitNcurses();

    FileManagerState st;
    InitFileManagerState(&st);

    char path_to_program_buf[PATH_MAX + 1];
    ConcatPaths(st.current_path, argv[0], path_to_program_buf);
    strcpy(g_path_to_program, path_to_program_buf);
    strcpy(g_argv0, argv[0]);
    int last_slash_idx = GetFileName(path_to_program_buf) - path_to_program_buf - 1;
    path_to_program_buf[last_slash_idx] = '\0';

    InitLibList(path_to_program_buf);

    if(args >= 2) {
        strcpy(st.current_path, argv[1]);
        st.current_path_len = strlen(st.current_path);
        ReloadCurrentDir(&st);
    }

    int key = 0;

    do {
        UpdateState(&st, key);
        erase();
        DisplayState(&st);
        refresh();
    } while ((key = getch()) != 'q');

    endwin();
    DestroyLibList();

    return 0;
}
