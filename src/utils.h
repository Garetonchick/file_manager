#pragma once

char* AllocCopy(const char* str);
char* AllocConcatPaths(const char* path, const char* local_path);
void ConcatPaths(const char* path, const char* local_path, char* out);
const char* GetFileName(const char* path);
const char* GetFileExtension(const char* path);