#include "display.h"

#include "structs.h"
#include "constants.h"
#include "global_buf.h"
#include "utils.h"

#include <ncurses.h>
#include <stdio.h>
#include <string.h>

void PrintSpaces(int num) {
    for (int i = 0; i < num; ++i) {
        printw(" ");
    }
}

int GetCurrentRow() {
    int x = 0;
    int y = 0;
    getyx(stdscr, y, x);

    (void)x;

    return y;
}

int GetCurrentColumn() {
    int x = 0;
    int y = 0;
    getyx(stdscr, y, x);

    (void)y;

    return x;
}

bool DisplayDirectoryContents(const FileManagerState* st) {
    int arrow_pos = st->selected_idx - st->first_item_idx;
    int items_left = st->items.size - st->first_item_idx;
    int cur_pos = 0;
    int row = GetCurrentRow() + 1;
    bool displayed_arrow = false;

    while(cur_pos < items_left && row < st->win_height) {
        move(row, 0);

        if(cur_pos == arrow_pos) {
            displayed_arrow = true;
            printw("->");
        } else {
            printw("  ");
        }

        DirItem* item = &st->items.items[st->first_item_idx + cur_pos];

        int color_pair = -1;

        if (item->type == FILE_TYPE_DIR) {
            color_pair = DIR_COLOR_PAIR;
        } else if(item->type == FILE_TYPE_SYMBOLIC_LINK) {
            color_pair = SYMLINK_COLOR_PAIR;
        } else if(item->type == FILE_TYPE_FIFO) {
            color_pair = FIFO_COLOR_PAIR;
        }

        if(color_pair != -1) {
            attron(COLOR_PAIR(color_pair));
            printw(item->name);
            attroff(COLOR_PAIR(color_pair));
        } else {
            printw(item->name);
        }

        row = GetCurrentRow();

        if(row >= st->win_height) {
            break;
        }

        if(st->first_item_idx + cur_pos) {
            sprintf(g_buf, "%zu", item->size);
            int digits_num = strlen(g_buf);

            int size_info_offset = (int)(SIZE_INFO_RELATIVE_OFFSET * (float)st->win_width) + strlen("Size");

            size_info_offset -= GetCurrentColumn() + digits_num;

            if(size_info_offset <= 0) {
                size_info_offset = 1;
            }

            PrintSpaces(size_info_offset);
            printw(g_buf);
        }

        row = GetCurrentRow();

        ++cur_pos;
        ++row;
    }

    return displayed_arrow;
}

void DisplayHeader(const FileManagerState* st) {
    int start_row = GetCurrentRow();

    move(start_row, 0);
    attron(A_REVERSE);

    PrintSpaces(st->win_width);

    move(start_row, 0);
    PrintSpaces(2);
    printw("Name");
    int size_info_offset = (int)(SIZE_INFO_RELATIVE_OFFSET * (float)st->win_width) - 2 - strlen("Size");
    PrintSpaces(size_info_offset);
    printw("Size");
    attroff(A_REVERSE);
}

void DisplayState(const FileManagerState* st) {
    move(0, 0);
    DisplayHeader(st);
    DisplayDirectoryContents(st);
}

bool CanArrowBeDisplayed(const FileManagerState* st) {
    if(st->selected_idx == st->first_item_idx) {
        return true;
    }

    move(0, 0);
    DisplayHeader(st);
    return DisplayDirectoryContents(st);
}