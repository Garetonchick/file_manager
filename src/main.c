#include "debug.h"
#include "actions.h"
#include "display.h"
#include "constants.h"
#include "extensions.h"
#include "global_buf.h"
#include "structs.h"
#include "utils.h"
#include "strings_storage.h"
#include "logger.h"

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

void InitNcurses() {
    initscr();
    curs_set(0);
    keypad(stdscr, true);
    nodelay(stdscr, true);

    use_default_colors();
    start_color();

    init_pair(DIR_COLOR_PAIR, COLOR_CYAN, -1);
    init_pair(SYMLINK_COLOR_PAIR, COLOR_MAGENTA, -1);
    init_pair(FIFO_COLOR_PAIR, COLOR_WHITE, COLOR_BLUE);

    refresh();

    Log("Initialized Ncurses\n");
}

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

char* AllocProgramDir(const char* argv0) {
    getcwd(g_buf, GLOBAL_BUF_SIZE);
    char* program_dir = AllocConcatPaths(g_buf, argv0);
    int last_slash_idx = GetFileName(program_dir) - program_dir - 1;

    program_dir[last_slash_idx] = '\0';

    return program_dir;
}

int main(int args, char* argv[]) {
    if(!args) {
        return ZERO_ARGS_FAIL_RETURN_CODE;
    }

    char* program_dir = AllocProgramDir(argv[0]); 

    InitLogger(program_dir);
    InitNcurses();

    FileManagerState st;
    InitFileManagerState(&st);

    InitLibList(program_dir);

    int key = 0;

    do {
        FetchWindowInfo(&st);
        UpdateState(&st, key);
        erase();
        DisplayState(&st);
        refresh();
        ClearStorageStrings();        
    } while ((key = getch()) != 'q');

    endwin();
    DestroyLibList();
    ClearStorage();
    DestroyLogger();

    return 0;
}
