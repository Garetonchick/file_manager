cmake_minimum_required(VERSION 3.16)
project(FileManger LANGUAGES C)

# Export compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Flags 
list(APPEND FILE_MANAGER_FLAGS
#    -O3
    -g
    -Wall
    -Wextra
    -Werror
)

# Ncurses
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Create main executable
add_executable(file_manager 
    src/main.c 
    src/actions.c 
    src/display.c 
    src/global_buf.c 
    src/structs.c 
    src/utils.c
)
target_link_libraries(file_manager ${CURSES_LIBRARIES})
target_compile_options(file_manager PRIVATE ${FILE_MANAGER_FLAGS})
message(STATUS "File manager flags are: ${FILE_MANAGER_FLAGS}")

# Setup clang-tidy 
set_target_properties(
    file_manager
    PROPERTIES
        CXX_CLANG_TIDY "clang-tidy"
)
