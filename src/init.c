#include "init.h"

#include "constants.h"

#include <ncurses.h>

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
}