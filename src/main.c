#include "debug.h"
#include "actions.h"
#include "display.h"
#include "constants.h"

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

    refresh();
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
            EnterDirAction(st);
            break;
        
        case 'D':
            DeleteDirMemberAction(st);
            break;

        default:
            break;
    }
}

int main(void) {
    InitNcurses();

    FileManagerState st;
    InitFileManagerState(&st);

    int key = 0;

    do {
        UpdateState(&st, key);
        erase();
        DisplayState(&st);
        refresh();
    } while ((key = getch()) != 'q');

    endwin();
    return 0;
}
