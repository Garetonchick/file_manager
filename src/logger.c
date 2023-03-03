#include "logger.h"

#include "global_buf.h"
#include "utils.h"
#include "exit.h"

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

static const char* log_file_name = "log.txt";
static FILE* g_log_file = NULL;

void InitLogger(const char* program_dir) {
    ConcatPaths(program_dir, log_file_name, g_buf);
    g_log_file = fopen(g_buf, "w");

    if(!g_log_file) {
        endwin();
        exit(LOGGER_INIT_FAILED_RETURN_CODE);
    }

    Log("Initialized log\n");
}

void DestroyLogger() {
    if(g_log_file) {
        fclose(g_log_file);
    }
}

void Log(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(g_log_file, format, args);
    va_end(args);
}