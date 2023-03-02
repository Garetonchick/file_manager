#include "display.h"

#include "structs.h"
#include "constants.h"
#include "global_buf.h"
#include "utils.h"

#include <ncurses.h>
#include <string.h>

void DisplayState(const FileManagerState* st) {
    int arrow_row = SUBWINDOW_ROW_OFFSET + st->selected_idx - st->first_item_idx;

    // Useful debug info
    mvprintfw(26, 1, "Cur path: %s", st->current_path);
    mvprintfw(27, 1, "Copy path: %s", st->copy_path);

    // Actual displays
    DisplayDirectoryContents(st->items, SUBWINDOW_ROWS, st->first_item_idx);
    DisplayArrow(arrow_row);
    DisplayHeader();
}

void DisplayDirectoryContents(DirItemsList content, int rows, int first_item_idx) {
    int lines_to_print = content.size;

    if (lines_to_print > rows) {
        lines_to_print = rows;
    }

    for (int i = 0; i < lines_to_print; ++i) {
        DirItem *item = &content.items[first_item_idx + i];

        if (item->is_dir) {
            attron(COLOR_PAIR(DIR_COLOR_PAIR));
            mvprintw(SUBWINDOW_ROW_OFFSET + i, SUBWINDOW_COL_OFFSET,
                     item->name);
            attroff(COLOR_PAIR(DIR_COLOR_PAIR));
        } else {
            mvprintw(SUBWINDOW_ROW_OFFSET + i, SUBWINDOW_COL_OFFSET,
                     item->name);
        }

        if(i) {
            sprintf(g_buf, "%zu", item->size);
            int digits_num = strlen(g_buf);
            mvprintw(SUBWINDOW_ROW_OFFSET + i, SIZE_INFO_COLUMN + 4 - digits_num, g_buf);
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
    mvprintw(0, SIZE_INFO_COLUMN, "Size");
    mvprintw(0, MODIFIED_INFO_COLUMN, "Modified");
    attroff(A_REVERSE);
}