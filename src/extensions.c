#include "extensions.h"

#include "constants.h"
#include "debug.h"
#include "init.h"
#include "structs.h"
#include "utils.h"
#include "init.h"

#include <string.h>
#include <dlfcn.h>
#include <ncurses.h>
#include <unistd.h>

struct LibList {
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
    PRINT_ERR("Ilist size: %d", ilist.size);

    for(int i = 0; i < ilist.size; ++i) {
        if(strcmp(GetFileExtension(ilist.items[i].name), ".so") == 0) {
            PRINT_ERR("Loading lib: %s\n", ilist.items[i].name);
            ConcatPaths(libs_dir, ilist.items[i].name, g_buf);
            void* handle = dlopen(g_buf, RTLD_LAZY);

            if(!handle) {
                continue;
            }

            GetExtensionsFnT get_extensions = dlsym(handle, "GetSupportedExtensions");

            if(!get_extensions) {
                dlclose(handle);
                continue;
            }

            int ext_num = 0;
            PRINT_ERR("Before get_extensions\n");
            const char** extensions = get_extensions(&ext_num);
            PRINT_ERR("After get_extensions\n");
            PRINT_ERR("Num of extensions: %d\n", ext_num);
            int libname_len = strlen(ilist.items[i].name);

            for(int j = 0; j < ext_num; ++j) {
                int ext_len = strlen(extensions[j]);
                g_libs.libs[g_libs.libs_num].extension = malloc(ext_len + 1);
                g_libs.libs[g_libs.libs_num].libname = malloc(libname_len + 1);

                strcpy(g_libs.libs[g_libs.libs_num].extension, extensions[j]);
                strcpy(g_libs.libs[g_libs.libs_num].libname, ilist.items[i].name);
                ++g_libs.libs_num;

                if(g_libs.libs_num == LIBS_LIST_SIZE) {
                    break;
                }
            }

            dlclose(handle);

            if(g_libs.libs_num == LIBS_LIST_SIZE) {
                break;
            }
        }
    }

    DestroyDirItemList(ilist);
}

void DestroyLibList() {
    free(g_libs.libs_dir);

    for(int i = 0; i < g_libs.libs_num; ++i) {
        free(g_libs.libs[i].extension);
        free(g_libs.libs[i].libname);
    }

    g_libs.libs_num = 0;
}

void OpenFile(const char* path, const char* current_path) {
    PRINT_ERR("Trying to open file: %s\n", path);
    const char* ext = GetFileExtension(path);
    const char* libname = NULL;

    for(int i = 0; i < g_libs.libs_num; ++i) {
        if(strcmp(ext, g_libs.libs[i].extension) == 0) {
            libname = g_libs.libs[i].libname;
            break;
        }
    }

    if(!libname) {
        return;
    }

    PRINT_ERR("Trying to load lib: %s\n", libname);

    ConcatPaths(g_libs.libs_dir, libname, g_buf);
    PRINT_ERR("Libs path: %s\n", g_buf);
    void* handle = dlopen(g_buf, RTLD_LAZY);

    if(!handle) {
        return;
    }

    PRINT_ERR("Loaded lib\n");

    OpenFileFnT open_file = dlsym(handle, "OpenFile");

    if(!open_file) {
        dlclose(handle);
        return;
    }

    PRINT_ERR("Opening file: %s\n", path);
    endwin();
    open_file(path);
    initscr();
    dlclose(handle);
    (void)current_path;

    // endwin();
    // execl(g_argv0, g_argv0, current_path, NULL);
    // exit(1);
}