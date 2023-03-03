#include "extensions.h"

#include "constants.h"
#include "structs.h"
#include "utils.h"
#include "global_buf.h"
#include "logger.h"
#include "exit.h"

#include <string.h>
#include <dlfcn.h>
#include <ncurses.h>
#include <unistd.h>
#include <errno.h>

typedef struct LibEntry {
    char* extension;
    char* libname;
} LibEntry;

static struct LibList {
    int libs_num;
    LibEntry libs[LIBS_LIST_SIZE];
    char* libs_dir;
} g_libs;

typedef const char** (*GetExtensionsFnT)(int*);
typedef void (*OpenFileFnT)(const char*);

void InitLibList(const char* libs_dir) { 
    g_libs.libs_num = 0;
    g_libs.libs_dir = malloc(strlen(libs_dir) + 1);
    strcpy(g_libs.libs_dir, libs_dir);

    DirItemsList ilist = GetDirItemsList(libs_dir, false);

    for(int i = 0; i < ilist.size; ++i) {
        if(strcmp(GetFileExtension(ilist.items[i].name), ".so") == 0) {
            Log("Found library: %s\n", ilist.items[i].name);
            ConcatPaths(libs_dir, ilist.items[i].name, g_buf);
            errno = 0;
            void* handle = dlopen(g_buf, RTLD_LAZY);

            if(!handle) {
                Log("Failed to open library: %s\n", ilist.items[i].name);
                Log("Reason: %s\n", strerror(errno));
                continue;
            }

            errno = 0;
            GetExtensionsFnT get_extensions = dlsym(handle, "GetSupportedExtensions");

            if(!get_extensions) {
                dlclose(handle);
                Log("Failed to use function GetSupportedExtensions from library: %s\n", ilist.items[i].name);
                Log("Reason: %s\n", strerror(errno));
                continue;
            }

            int ext_num = 0;
            const char** extensions = get_extensions(&ext_num);
            int libname_len = strlen(ilist.items[i].name);

            for(int j = 0; j < ext_num; ++j) {
                int ext_len = strlen(extensions[j]);
                g_libs.libs[g_libs.libs_num].extension = malloc(ext_len + 1);
                g_libs.libs[g_libs.libs_num].libname = malloc(libname_len + 1);

                if(!g_libs.libs[g_libs.libs_num].extension || 
                   !g_libs.libs[g_libs.libs_num].libname) {
                    FailExit();
                }

                strcpy(g_libs.libs[g_libs.libs_num].extension, extensions[j]);
                strcpy(g_libs.libs[g_libs.libs_num].libname, ilist.items[i].name);
                ++g_libs.libs_num;

                if(g_libs.libs_num == LIBS_LIST_SIZE) {
                    break;
                }
            }

            dlclose(handle);
            Log("Loaded library: %s\n", ilist.items[i].name);

            if(g_libs.libs_num == LIBS_LIST_SIZE) {
                break;
            }
        }
    }

    DestroyDirItemList(ilist);

    Log("Initialized library list\n");
}

void DestroyLibList() {
    free(g_libs.libs_dir);

    for(int i = 0; i < g_libs.libs_num; ++i) {
        free(g_libs.libs[i].extension);
        free(g_libs.libs[i].libname);
    }

    g_libs.libs_num = 0;
}

void OpenFile(const char* path) {
    const char* ext = GetFileExtension(path);
    const char* libname = NULL;

    for(int i = 0; i < g_libs.libs_num; ++i) {
        if(strcmp(ext, g_libs.libs[i].extension) == 0) {
            libname = g_libs.libs[i].libname;
            break;
        }
    }

    if(!libname) {
        Log("Couldn't find library to load file: %s\n", path);
        return;
    }

    ConcatPaths(g_libs.libs_dir, libname, g_buf);
    errno = 0;
    void* handle = dlopen(g_buf, RTLD_LAZY);

    if(!handle) {
        Log("Failed to open library: %s\n", libname);
        Log("Reason: %s\n", strerror(errno));
        return;
    }

    errno = 0;
    OpenFileFnT open_file = dlsym(handle, "OpenFile");

    if(!open_file) {
        dlclose(handle);
        Log("Failed to use OpenFile function from library: %s\n", libname);
        Log("Reason: %s\n", strerror(errno));
        return;
    }

    open_file(path);
    endwin();
    initscr();
    dlclose(handle);
}