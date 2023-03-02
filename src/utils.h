#pragma once

#include "constants.h"

#include "global_buf.h"

#include <stdio.h>
#include <ncurses.h>

void ConcatPaths(const char* path, const char* local_path, char* out);
const char* GetFileName(const char* path);

void mvprintfw(int row, int col, const char* format, ...); // NOLINT