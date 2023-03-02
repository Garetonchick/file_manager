#pragma once
#include "structs.h"

void SelectFileBelowAction(FileManagerState* st);
void SelectFileAboveAction(FileManagerState* st);
void EnterPressedAction(FileManagerState* st);
void DeleteDirMemberAction(FileManagerState* st);
void CutCopyFile(FileManagerState* st);
void CopyFile(FileManagerState* st);
void PasteFile(FileManagerState* st);
void SwitchShowingHidden(FileManagerState* st);