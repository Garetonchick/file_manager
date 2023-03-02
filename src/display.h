#pragma once
#include "structs.h"

void DisplayState(const FileManagerState* st);
void DisplayDirectoryContents(DirItemsList content, int rows, int first_item_idx);
void DisplayArrow(int row);
void DisplayHeader();