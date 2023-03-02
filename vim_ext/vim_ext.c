#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <sys/wait.h>

void OpenFile(const char* path) {
    static char command[PATH_MAX + 6];
    snprintf(command, sizeof(command), "vim %s", path);

    pid_t pid = fork();

    if(pid == -1) {
        return;
    } else if(pid == 0) {
        execlp("vim", "vim", path, NULL);
        exit(0);
    } else {
        wait(NULL);
    }
}

const char** GetSupportedExtensions(int* num) {
    static const char* extenstions[] = {
        ".txt",
        ".kek",
        ".lol"
    };

    *num = 3;

    return extenstions;
}