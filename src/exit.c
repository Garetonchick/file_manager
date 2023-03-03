#include "exit.h"

#include "logger.h"
#include "constants.h"

#include <ncurses.h>
#include <stdlib.h>

void FailExit() {
    Log("FailExit\n");
    endwin();
    exit(AVERAGE_FAIL_ENJOYER_RETURN_CODE);
}